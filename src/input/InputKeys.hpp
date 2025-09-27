#pragma once

#include "input/InputDevice.hpp"
#include "input/InputKeyCodes.hpp"

namespace v3d {
namespace input {

constexpr InputKeyResult IKey_Release{GLFW_RELEASE};
constexpr InputKeyResult IKey_Press{GLFW_PRESS};
constexpr InputKeyResult IKey_Repeat{GLFW_REPEAT};
constexpr InputKeyResult IKey_None{-1};

namespace action {
constexpr InputAction IAct_Accelerate = makeInputActionID("Accelerate");
constexpr InputAction IAct_Back = makeInputActionID("Back");
constexpr InputAction IAct_Break = makeInputActionID("Break");
constexpr InputAction IAct_Clutch = makeInputActionID("Clutch");
constexpr InputAction IAct_SteerLeft = makeInputActionID("SteerLeft");
constexpr InputAction IAct_SteerRight = makeInputActionID("SteerRight");

}  // namespace action

}  // namespace input

}  // namespace v3d
