#pragma once

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace v3d {
class Entity;
class Scene;

template <typename Container, typename T, typename key = boost::uuids::uuid>
class object_ptr {
   public:
    object_ptr() : m_vec(nullptr), m_index(boost::uuids::nil_uuid()) {}
    object_ptr(Container& vec, key index)
        : m_vec(&vec), m_index(index) {
        // assert(index < static_cast<key>(m_vec->size()) && "object_ptr: index out of bounds at construction");
    }
    // object_ptr(Container& vec, T* ptr) : m_vec(&vec) {
    //     // // Check if the pointer is within the vector's bounds
    //     // if (ptr < vec.data() || ptr >= vec.data() + vec.size()) {
    //     //     throw std::out_of_range("vector_observer: pointer is out of vector bounds");
    //     // }

    //     // Compute index from pointer
    //     m_index = static_cast<IndexType>(ptr - vec.data());
    // }

    // Copy assignment
    object_ptr& operator=(const object_ptr& other) noexcept {
        if (this != &other) {  // Avoid self-assignment
            m_vec = other.m_vec;
            m_index = other.m_index;
        }
        return *this;
    }

    // Assignment index
    object_ptr& operator=(const key& other) noexcept {
        if (other == boost::uuids::nil_uuid()) {
            reset();
        } else {
            m_index = other.m_index;
        }
        return *this;
    }

    // Assign nullptr
    object_ptr& operator=(const std::nullptr_t& other) noexcept {
        reset();
        return *this;
    }

    // Overload equality operator to check for null state
    bool operator==(const key& other) const noexcept {
        return m_index == other;
    }

    // Overload inequality operator
    bool operator!=(const key& other) const noexcept {
        return !(m_index == other);
    }

    // Overload equality operator to check for null state
    bool operator==(const std::nullptr_t& other) const noexcept {
        return m_index == boost::uuids::nil_uuid();
    }

    // Overload inequality operator
    bool operator!=(const std::nullptr_t& other) const noexcept {
        return !(m_index == boost::uuids::nil_uuid());
    }

    bool operator==(const object_ptr& other) const noexcept {
        return m_vec == other.m_vec && m_index == other.m_index;
    }

    bool operator!=(const object_ptr& other) const noexcept {
        return !(m_vec == other.m_vec && m_index == other.m_index);
    }

    explicit operator bool() const noexcept {
        return m_vec && m_index != boost::uuids::nil_uuid();
    }

    // Accessors
    T& get() {
        // assert(valid() && "object_ptr: accessing an invalid or out-of-bounds index");
        return (*m_vec)[m_index];
    }

    const T& get() const {
        // assert(valid() && "object_ptr: accessing an invalid or out-of-bounds index");
        return (*m_vec)[m_index];
    }

    T* operator->() {
        return &get();
    }

    T& operator*() {
        return get();
    }

    key index() const noexcept {
        return m_index;
    }

    void reset() noexcept {
        m_vec = nullptr;
        m_index = boost::uuids::nil_uuid();
    }

    // void update_index(key new_index) noexcept {
    //     assert(new_index < static_cast<key>(m_vec->size()) && "object_ptr: new index out of bounds");
    //     m_index = new_index;
    // }

    // // Checks if is index within bounds
    // inline bool valid() const noexcept {
    //     return m_vec && m_index < m_vec->size();
    // }

   private:
    Container* m_vec;
    key m_index;
};

}  // namespace v3d
