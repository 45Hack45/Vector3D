#pragma once

#include <boost/unordered/unordered_flat_map.hpp>
#include <cassert>
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>

namespace v3d {
namespace utils {

template <typename Key, typename Base>
class KeyedStableCollection {
   public:
    struct Handle {
        std::type_index type;
        std::size_t index;
        std::size_t generation;

        Handle() : type(typeid(void)), index(0), generation(0) {}

        Handle(std::type_index t, std::size_t i, std::size_t g)
            : type(t), index(i), generation(g) {}

        bool operator==(const Handle& other) const {
            return type == other.type && index == other.index && generation == other.generation;
        }
    };

    KeyedStableCollection() = default;

    template <typename Derived, typename... Args>
    bool insert(const Key& key, Args&&... args) {
        static_assert(std::is_base_of<Base, Derived>::value, "Must inherit from Base");

        if (m_keyToHandle.contains(key)) return false;

        auto& typed = getStorage<Derived>();
        typed.entries.emplace_back(Derived{std::forward<Args>(args)...});
        typed.generations.push_back(0);

        Handle handle{typeid(Derived), typed.entries.size() - 1, 0};
        m_keyToHandle[key] = handle;
        return true;
    }

    Base* get(const Key& key) {
        auto it = m_keyToHandle.find(key);
        if (it == m_keyToHandle.end()) return nullptr;
        return getRaw(it->second);
    }

    template <typename Derived>
    Derived* getAs(const Key& key) {
        static_assert(std::is_base_of<Base, Derived>::value);
        auto it = m_keyToHandle.find(key);
        if (it == m_keyToHandle.end()) return nullptr;

        const Handle& h = it->second;
        if (h.type != typeid(Derived)) return nullptr;

        auto& storage = getStorage<Derived>();
        if (h.index >= storage.entries.size() || h.generation != storage.generations[h.index])
            return nullptr;

        return &storage.entries[h.index];
    }

    bool erase(const Key& key) {
        auto it = m_keyToHandle.find(key);
        if (it == m_keyToHandle.end()) return false;

        const Handle& handle = it->second;
        auto storageIt = m_derivedStorage.find(handle.type);
        if (storageIt == m_derivedStorage.end()) return false;

        auto& genVec = storageIt->second->generations;

        if (handle.index >= genVec.size() || genVec[handle.index] != handle.generation)
            return false;

        storageIt->second->erase(handle.index);
        m_keyToHandle.erase(it);
        return true;
    }

    void compact() {
        for (auto& [type, storage] : m_derivedStorage) {
            storage->compact();
        }
        remapHandles();
    }

    template <typename Func>
    void for_each(Func&& func) {
        for (auto& [type, storage] : m_derivedStorage) {
            storage->for_each(std::forward<Func>(func));
        }
    }

   private:
    struct TypedVectorBase {
        TypedVectorBase() = default;
        virtual ~TypedVectorBase() = default;

        std::vector<std::size_t> generations;
        virtual Base* get(std::size_t idx) = 0;
        virtual void erase(std::size_t idx) = 0;
        virtual void compact() = 0;
        virtual void for_each(std::function<void(Base&)>&& func) = 0;
    };

    template <typename Derived>
    struct TypedVector : TypedVectorBase {
        std::deque<Derived> entries;

        Base* get(std::size_t idx) override {
            if (idx >= entries.size() || this->generations[idx] == static_cast<std::size_t>(-1))
                return nullptr;
            return &entries[idx];
        }

        void erase(std::size_t idx) override {
            if (idx < entries.size()) {
                entries[idx] = Derived();  // Optional: reset object
                this->generations[idx]++;
            }
        }

        void compact() override {
            std::deque<Derived> newEntries;
            std::vector<std::size_t> newGenerations;

            for (std::size_t i = 0; i < entries.size(); ++i) {
                if (this->generations[i] != static_cast<std::size_t>(-1)) {
                    newEntries.push_back(std::move(entries[i]));
                    newGenerations.push_back(this->generations[i]);
                }
            }

            std::swap(entries, newEntries);
            std::swap(this->generations, newGenerations);
        }

        void for_each(std::function<void(Base&)>&& func) override {
            for (std::size_t i = 0; i < entries.size(); ++i) {
                if (this->generations[i] != static_cast<std::size_t>(-1)) {
                    func(entries[i]);
                }
            }
        }
    };

    template <typename Derived>
    TypedVector<Derived>& getStorage() {
        auto type = std::type_index(typeid(Derived));
        auto it = m_derivedStorage.find(type);
        if (it == m_derivedStorage.end()) {
            auto ptr = std::make_unique<TypedVector<Derived>>();
            auto& ref = *ptr;
            m_derivedStorage[type] = std::move(ptr);
            return ref;
        }
        return *static_cast<TypedVector<Derived>*>(m_derivedStorage[type].get());
    }

    Base* getRaw(const Handle& h) {
        auto it = m_derivedStorage.find(h.type);
        if (it == m_derivedStorage.end()) return nullptr;

        auto& storage = *it->second;
        if (h.index >= storage.generations.size() || h.generation != storage.generations[h.index])
            return nullptr;

        return storage.get(h.index);
    }

    void remapHandles() {
        for (auto& [key, handle] : m_keyToHandle) {
            auto storageIt = m_derivedStorage.find(handle.type);
            if (storageIt == m_derivedStorage.end()) continue;

            auto& genVec = storageIt->second->generations;

            if (handle.index < genVec.size() && genVec[handle.index] == handle.generation) {
                continue;
            }

            for (std::size_t newIdx = 0; newIdx < genVec.size(); ++newIdx) {
                if (genVec[newIdx] == handle.generation) {
                    handle.index = newIdx;
                    break;
                }
            }
        }
    }

    boost::unordered_flat_map<Key, Handle> m_keyToHandle;
    std::unordered_map<std::type_index, std::unique_ptr<TypedVectorBase>> m_derivedStorage;
};

}  // namespace utils
}  // namespace v3d
