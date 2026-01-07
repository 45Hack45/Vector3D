#pragma once

#include <boost/unordered/unordered_flat_map.hpp>
#include <cassert>
#include <deque>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace v3d {
namespace utils {

/// @brief A collection that stores polymorphic objects (derived from a common
/// base) with a stable handle system and key-based lookup.
/// @tparam Key Index
/// @tparam Base Polymorfic Base class
template <typename Key, typename Base>
class KeyedStableCollection {
   public:
    // A handle representing a reference to a stored object, including its type,
    // index in the storage container, and generation for versioning.
    struct Handle {
        std::type_index type;
        std::size_t index;
        std::size_t generation;

        Handle() : type(typeid(void)), index(0), generation(0) {}

        Handle(std::type_index t, std::size_t i, std::size_t g)
            : type(t), index(i), generation(g) {}

        // Equality operator to compare handles
        bool operator==(const Handle& other) const {
            return type == other.type && index == other.index &&
                   generation == other.generation;
        }
    };

    KeyedStableCollection() = default;

    // Insert a new Derived object constructed with Args... under the specified
    // key. Fails if the key already exists.
    template <typename Derived, typename... Args>
    bool insert(const Key& key, Args&&... args) {
        static_assert(std::is_base_of<Base, Derived>::value,
                      "Must inherit from Base");

        if (m_keyToHandle.contains(key)) return false;

        auto& typed = getStorage<Derived>();
        typed.entries.emplace_back(Derived{std::forward<Args>(args)...});
        typed.generations.push_back(0);

        Handle handle{typeid(Derived), typed.entries.size() - 1, 0};
        m_keyToHandle[key] = handle;
        return true;
    }

    /// @brief Insert a constructed instance of Derived under the specified key,
    /// taking ownership of the instance. Note: Derived must be registered
    /// before inserting any instance of Derived.
    /// @param key index
    /// @param derived Instance of the Derived class upcasted to Base.
    /// @return True if inserted succesfully, False if key already has value.
    bool insert(const Key& key, std::unique_ptr<Base> derived) {
        if (m_keyToHandle.contains(key)) return false;

        auto derivedType = std::type_index(typeid(*derived));

        auto storage = getStorage(derivedType);

        // using Dep = std::decay_t<decltype(derived.get())>;
        // // auto& storage = getStorage<Dep>();

        storage->push_back(std::move(derived));
        storage->generations.push_back(0);

        auto size = storage->size();

        Handle handle{derivedType, size - 1, 0};
        m_keyToHandle[key] = handle;
        return true;
    }

    // Get a raw Base* to the object associated with the key (if exists).
    Base* get(const Key& key) {
        auto it = m_keyToHandle.find(key);
        if (it == m_keyToHandle.end()) return nullptr;
        return getRaw(it->second);
    }

    // Get a Derived* to the object associated with the key, only if the type
    // matches.
    template <typename Derived>
    Derived* getAs(const Key& key) {
        static_assert(std::is_base_of<Base, Derived>::value);
        auto it = m_keyToHandle.find(key);
        if (it == m_keyToHandle.end()) return nullptr;

        const Handle& h = it->second;
        if (h.type != typeid(Derived)) return nullptr;

        auto& storage = getStorage<Derived>();
        if (h.index >= storage.entries.size() ||
            h.generation != storage.generations[h.index])
            return nullptr;

        return &storage.entries[h.index];
    }

    template <typename Derived>
    Derived* getFirstOfType() {
        static_assert(std::is_base_of<Base, Derived>::value);
        auto& derivedStorage = getStorage<Derived>();

        return &derivedStorage.entries.front();
    }

    // Erase the object associated with the given key, if it exists.
    bool erase(const Key& key) {
        auto it = m_keyToHandle.find(key);
        if (it == m_keyToHandle.end()) return false;

        const Handle& handle = it->second;
        auto storageIt = m_derivedStorage.find(handle.type);
        if (storageIt == m_derivedStorage.end()) return false;

        auto& genVec = storageIt->second->generations;

        if (handle.index >= genVec.size() ||
            genVec[handle.index] != handle.generation)
            return false;

        storageIt->second->erase(handle.index);
        m_keyToHandle.erase(it);
        return true;
    }

    // Compact all internal storage to remove gaps left by deleted entries.
    void compact() {
        for (auto& [type, storage] : m_derivedStorage) {
            storage->compact();
        }
        remapHandles();  // Recalculate handles after compacting
    }

    // Apply a function to all stored objects.
    template <typename Func>
    void for_each(Func&& func) {
        for (auto& [type, storage] : m_derivedStorage) {
            storage->for_each(std::forward<Func>(func));
        }
    }

    // Abstract base class for storing derived objects.
    struct TypedVectorBase {
        TypedVectorBase() = default;
        virtual ~TypedVectorBase() = default;

        std::vector<std::size_t> generations;

        virtual Base* get(std::size_t idx) = 0;
        virtual std::size_t size() = 0;
        virtual void erase(std::size_t idx) = 0;
        virtual void push_back(std::unique_ptr<Base> derived) = 0;
        virtual void compact() = 0;
        virtual void for_each(std::function<void(Base&)>&& func) = 0;
    };

    // Template implementation of TypedVectorBase for a specific derived type.
    template <typename Derived>
    struct TypedVector : TypedVectorBase {
        TypedVector() {
            static_assert(std::is_copy_constructible_v<Derived> ||
                              std::is_move_constructible_v<Derived>,
                          "Derived must be copy or move constructible");

            static_assert(std::is_copy_assignable_v<Derived> ||
                              std::is_move_assignable_v<Derived>,
                          "Derived must be copy or move assignable");
        }
        std::deque<Derived> entries;

        Base* get(std::size_t idx) override {
            if (idx >= entries.size() ||
                this->generations[idx] == static_cast<std::size_t>(-1))
                return nullptr;
            return &entries[idx];
        }

        std::size_t size() override { return entries.size(); }

        void erase(std::size_t idx) override {
            if (idx < entries.size()) {
                entries[idx] = std::move(Derived());  // Reset the object
                this->generations[idx]++;
            }
        }

        void push_back(std::unique_ptr<Base> derived) override {
            Derived* ptr = static_cast<Derived*>(derived.release());
            entries.push_back(std::move(*ptr));
        }

        // Remove unused entries and update generations vector accordingly.
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

        // Call the provided function on all valid entries.
        void for_each(std::function<void(Base&)>&& func) override {
            for (std::size_t i = 0; i < entries.size(); ++i) {
                if (this->generations[i] != static_cast<std::size_t>(-1)) {
                    func(entries[i]);
                }
            }
        }
    };

    /// @brief Register type, initialize the internal container for the Derived
    /// type. Required for inserting when the Derived type is not known at
    /// compile time, call before first insertion.
    /// @tparam Derived
    template <typename Derived>
    void registerType() {
        auto typeIndex = std::type_index(typeid(Derived));
        auto it = m_derivedStorage.find(typeIndex);

        if (it != m_derivedStorage.end()) {
            // Already registered
            return;
        }

        auto ptr = std::make_unique<TypedVector<Derived>>();
        m_derivedStorage[typeIndex] = std::move(ptr);
    }

    /// @brief Register type, use provided container for typeIndex. Required for
    /// inserting when the Derived type is not known at compile time, call
    /// before first insertion.
    /// @param typeIndex The Derived type (std::type_index) of typeVector    
    /// @param typeVector Container for Derived type
    void registerType(
        std::type_index typeIndex,
        std::unique_ptr<TypedVectorBase>
            typeVector) {
        m_derivedStorage[typeIndex] = std::move(typeVector);
    }

   private:
    // Get (or create if not present) the TypedVector for a specific Derived
    // type.
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
        return *static_cast<TypedVector<Derived>*>(
            m_derivedStorage[type].get());
    }

    TypedVectorBase* getStorage(std::type_index type) {
        auto it = m_derivedStorage.find(type);
        if (it == m_derivedStorage.end()) {
            return nullptr;
        }
        return static_cast<TypedVectorBase*>(m_derivedStorage[type].get());
    }

    // Get a raw pointer to a Base using a Handle.
    Base* getRaw(const Handle& h) {
        auto it = m_derivedStorage.find(h.type);
        if (it == m_derivedStorage.end()) return nullptr;

        auto& storage = *it->second;
        if (h.index >= storage.generations.size() ||
            h.generation != storage.generations[h.index])
            return nullptr;

        return storage.get(h.index);
    }

    // Update all handles after a compaction, so they point to correct new
    // indices.
    void remapHandles() {
        for (auto& [key, handle] : m_keyToHandle) {
            auto storageIt = m_derivedStorage.find(handle.type);
            if (storageIt == m_derivedStorage.end()) continue;

            auto& genVec = storageIt->second->generations;

            if (handle.index < genVec.size() &&
                genVec[handle.index] == handle.generation) {
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

    // Maps a key to a handle referencing the actual object.
    boost::unordered_flat_map<Key, Handle> m_keyToHandle;

    // Maps type_info to typed storage containers.
    std::unordered_map<std::type_index, std::unique_ptr<TypedVectorBase>>
        m_derivedStorage;
};

}  // namespace utils
}  // namespace v3d
