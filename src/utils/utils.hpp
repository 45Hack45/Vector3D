#pragma once

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace v3d {
namespace utils {
uint32_t inline convertToUint32(int value) {
    if (value < 0) {
        throw std::invalid_argument("Value must be non-negative.");
    }
    int test = static_cast<int>(std::numeric_limits<uint32_t>::max());
    bool overflow = value > test;
    if (value > static_cast<int>(std::numeric_limits<uint32_t>::max())) {
        throw std::out_of_range("Value exceeds uint32_t range.");
    }
    return static_cast<uint32_t>(value);
}

// /**
//  * @brief Helper function to combine a given hash
//  *        with a generated hash for the input param.
//  */
// template <class T>
// inline void hash_combine(size_t &seed, const T &v)
// {
// 	std::hash<T> hasher;
// 	glm::detail::hash_combine(seed, hasher(v));
// }

/**
 * @brief Helper function to convert a data type
 *        to string using output stream operator.
 * @param value The object to be converted to string
 * @return String version of the given object
 */
template <class T>
inline std::string to_string(const T &value) {
    std::stringstream ss;
    ss << std::fixed << value;
    return ss.str();
}

/**
 * @brief Helper function to check size_t is correctly converted to uint32_t
 * @param value Value of type size_t to convert
 * @return An uint32_t representation of the same value
 */
template <class T>
uint32_t to_u32(T value) {
    static_assert(std::is_arithmetic<T>::value, "T must be numeric");

    if (static_cast<uintmax_t>(value) >
        static_cast<uintmax_t>(std::numeric_limits<uint32_t>::max())) {
        throw std::runtime_error(
            "to_u32() failed, value is too big to be converted to uint32_t");
    }

    return static_cast<uint32_t>(value);
}

template <typename T>
inline std::vector<uint8_t> to_bytes(const T &value) {
    return std::vector<uint8_t>{
        reinterpret_cast<const uint8_t *>(&value),
        reinterpret_cast<const uint8_t *>(&value) + sizeof(T)};
}

template <typename Tuple, typename Func, std::size_t... I>
void forEachInTupleImpl(Func &&func, std::index_sequence<I...>) {
    (func(std::tuple_element_t<I, Tuple>{}), ...);
}

template <typename Tuple, typename Func>
void forEachInTuple(Func &&func) {
    forEachInTupleImpl<Tuple>(
        std::forward<Func>(func),
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

template <typename Tuple, typename Func>
void forEachInTuple(Tuple &&, Func &&func) {
    using T = std::decay_t<Tuple>;
    forEachInTupleImpl<T>(std::forward<Func>(func),
                          std::make_index_sequence<std::tuple_size_v<T>>{});
}

// Hash function
constexpr uint64_t fnv1a_64(std::string_view str) {
    uint64_t hash = 1469598103934665603ull;  // FNV offset basis
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ull;  // FNV prime
    }
    return hash;
}

}  // namespace utils
}  // namespace v3d
