#include <cstdint>
#include <stdexcept>

namespace v3d {
namespace utils {
uint32_t convertToUint32(int value) {
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
inline std::string to_string(const T &value)
{
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
uint32_t to_u32(T value)
{
	static_assert(std::is_arithmetic<T>::value, "T must be numeric");

	if (static_cast<uintmax_t>(value) > static_cast<uintmax_t>(std::numeric_limits<uint32_t>::max()))
	{
		throw std::runtime_error("to_u32() failed, value is too big to be converted to uint32_t");
	}

	return static_cast<uint32_t>(value);
}

template <typename T>
inline std::vector<uint8_t> to_bytes(const T &value)
{
	return std::vector<uint8_t>{reinterpret_cast<const uint8_t *>(&value),
	                            reinterpret_cast<const uint8_t *>(&value) + sizeof(T)};
}


}  // namespace utils
}  // namespace v3d
