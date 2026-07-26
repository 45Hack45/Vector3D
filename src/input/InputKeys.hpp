#pragma once

#include "input/InputDevice.hpp"
#include "input/InputKeyCodes.h"

namespace v3d {
namespace input {

constexpr InputKeyResult IKey_Release{GLFW_RELEASE};
constexpr InputKeyResult IKey_Press{GLFW_PRESS};
constexpr InputKeyResult IKey_Repeat{GLFW_REPEAT};
constexpr InputKeyResult IKey_None{-1};

// Source of truth for built-in actions: each entry emits the constexpr constant
// and the registry entry. The argument is the persisted name.
#define V3D_INPUT_ACTIONS(V3D_X)                                      \
    V3D_X(Accelerate) V3D_X(Back) V3D_X(Brake) V3D_X(Clutch)          \
    V3D_X(SteerLeft) V3D_X(SteerRight)

namespace action {

#define V3D_INPUT_ACTION_CONSTANT(name) \
    constexpr InputAction IAct_##name = makeInputActionID(#name);
V3D_INPUT_ACTIONS(V3D_INPUT_ACTION_CONSTANT)
#undef V3D_INPUT_ACTION_CONSTANT

}  // namespace action

/// @brief Register every built-in action name. Bindings naming an unregistered
/// action stay unresolved until it is registered and applyConfig() runs again.
/// @throws std::logic_error on a hash collision between action names.
void registerBuiltinActions();

}  // namespace input

}  // namespace v3d
