#include <stdexcept>

namespace v3d {
namespace exception {

class NotImplemented : public std::logic_error {
   public:
    NotImplemented() : std::logic_error("Function not yet implemented") {};
};

}  // namespace exception
}  // namespace v3d
