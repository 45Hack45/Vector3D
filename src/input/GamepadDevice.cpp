#include "input/GamepadDevice.h"

#include <algorithm>
#include <cmath>

#include "input/InputKeyCodes.h"

namespace v3d {
namespace input {

namespace {

// Rescale so the value ramps from 0 at the deadzone edge.
float applyDeadzone(float value, float deadzone) {
    deadzone = std::clamp(deadzone, 0.0f, 0.99f);
    if (value <= deadzone) return 0.0f;
    return (value - deadzone) / (1.0f - deadzone);
}

}  // namespace

GamepadDevice::GamepadDevice(Window* window, int joystickId, std::string guid,
                             std::string name, InputProfile profile)
    : InputDevice(window, std::move(guid), std::move(name), std::move(profile)),
      m_joystickId(joystickId) {}

void GamepadDevice::update() {
    if (!glfwGetGamepadState(m_joystickId, &m_state)) m_state = GLFWgamepadstate{};
}

float GamepadDevice::getRawInput(InputKey key) const {
    switch (key.kind) {
        case InputKeyKind::GamepadButton: {
            if (key.code < 0 || key.code > GLFW_GAMEPAD_BUTTON_LAST) return 0.0f;
            return m_state.buttons[key.code] == GLFW_PRESS ? 1.0f : 0.0f;
        }
        case InputKeyKind::GamepadAxis: {
            if (key.code < 0 || key.code > GLFW_GAMEPAD_AXIS_LAST) return 0.0f;
            return m_state.axes[key.code];
        }
        case InputKeyKind::Keyboard:
            break;
    }
    return 0.0f;
}

float GamepadDevice::getRawInput(InputAction action) const {
    const std::vector<BoundInput>* inputs = m_profile.getInputs(action);
    if (!inputs) return 0.0f;

    // Largest deflection from zero wins; raw values share no common scale.
    // A bound trigger reads -1 at rest.
    float value = 0.0f;
    for (const BoundInput& input : *inputs) {
        float raw = getRawInput(input.key);
        if (std::abs(raw) > std::abs(value)) value = raw;
    }
    return value;
}

float GamepadDevice::readInput(BoundInput input) const {
    float value = getRawInput(input.key);

    // Buttons are already 0 or 1; the axis settings do not apply to them.
    if (input.key.kind != InputKeyKind::GamepadAxis) return value;

    // Triggers rest at -1 and run to +1; map onto [0, 1] before anything else
    // reads the magnitude.
    if (isTriggerAxis(input.key)) value = (value + 1.0f) * 0.5f;

    // Each half of an axis is bound separately; the other half reads 0.
    if (input.direction == AxisDirection::Negative) value = -value;
    if (value <= 0.0f) return 0.0f;

    return applyDeadzone(value, input.deadzone);
}

float GamepadDevice::getInput(InputAction action) const {
    // TODO: per-action processing (response curves, smoothing)
    const std::vector<BoundInput>* inputs = m_profile.getInputs(action);
    if (!inputs) return 0.0f;

    float value = 0.0f;
    for (const BoundInput& input : *inputs) {
        value = std::max(value, readInput(input));
    }
    return value;
}

InputKeyResult GamepadDevice::getKey(InputKey key) const {
    if (key.kind != InputKeyKind::GamepadButton) return IKey_None;
    if (key.code < 0 || key.code > GLFW_GAMEPAD_BUTTON_LAST) return IKey_None;
    return m_state.buttons[key.code] == GLFW_PRESS ? IKey_Press : IKey_Release;
}

}  // namespace input

}  // namespace v3d
