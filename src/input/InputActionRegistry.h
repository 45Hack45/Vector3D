#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "input/InputDevice.hpp"

namespace v3d {
namespace input {

struct InputActionInfo {
    std::string name;  // "Accelerate"
    InputAction action;
};

/// @brief Name <-> InputAction mapping.
class InputActionRegistry {
   public:
    InputActionRegistry() = default;
    ~InputActionRegistry() = default;

    static InputActionRegistry& instance();

    /// @brief Register an action name. Called once per action at startup.
    /// @throws std::logic_error if the name's hash collides with a different
    ///         registered name. Re-registering the same name is a no-op.
    InputAction registerAction(std::string name);

    /// @brief Resolve a name read from a KeyBinding, or empty if unknown.
    std::optional<InputAction> find(std::string_view name) const;

    /// @brief The name an action was registered under, or nullptr.
    const std::string* nameOf(InputAction action) const;

    /// @brief Info of every registered action.
    std::vector<InputActionInfo> getAllInfo() const;

   private:
    std::unordered_map<uint64_t, std::string> m_actionRegistry;
};

}  // namespace input

}  // namespace v3d
