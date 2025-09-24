#include <cassert>
#include <stdexcept>
#include <vector>

namespace v3d {
namespace utils {

template <typename T, typename key = size_t>
class vector_ptr {
   public:
    vector_ptr(std::vector<T>& vec, key index)
        : m_vec(&vec), m_index(index) {
        assert(index < static_cast<key>(m_vec->size()) && "vector_ptr: index out of bounds at construction");
    }
    vector_ptr(std::vector<T>& vec, T* ptr) : m_vec(&vec) {
        // Check if the pointer is within the vector's bounds
        if (ptr < vec.data() || ptr >= vec.data() + vec.size()) {
            throw std::out_of_range("vector_observer: pointer is out of vector bounds");
        }

        // Compute index from pointer
        m_index = static_cast<IndexType>(ptr - vec.data());
    }

    // Accessors
    T& get() {
        assert(valid() && "vector_ptr: accessing an invalid or out-of-bounds index");
        return (*m_vec)[m_index];
    }

    const T& get() const {
        assert(valid() && "vector_ptr: accessing an invalid or out-of-bounds index");
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

    void update_index(key new_index) noexcept {
        assert(new_index < static_cast<key>(m_vec->size()) && "vector_ptr: new index out of bounds");
        m_index = new_index;
    }

    // Checks if  is index within bounds
    inline bool valid() const noexcept {
        return m_vec && m_index < m_vec->size();
    }

   private:
    std::vector<T>* m_vec;
    key m_index;
};

}  // namespace utils
}  // namespace v3d
