#include "input/MouseDevice.h"

#include <algorithm>
#include <cmath>

#include "input/InputKeyCodes.h"

namespace v3d {
namespace input {

MouseDevice::MouseDevice(Window* window, InputProfile profile)
    : InputDevice(window, std::string(kMouseDeviceGuid),
                  std::string(kMouseDeviceName), std::move(profile)) {}

void MouseDevice::accumulateScroll(float xoffset, float yoffset) {
    m_scrollAccum[0] += xoffset;
    m_scrollAccum[1] += yoffset;
}

void MouseDevice::update() {
    // The accumulator is drained exactly once per update
    const float scrollX = m_scrollAccum[0];
    const float scrollY = m_scrollAccum[1];
    m_scrollAccum[0] = 0.0f;
    m_scrollAccum[1] = 0.0f;

    GLFWwindow* window = m_window->getWindow();

    // The cursor only tracks this window while it holds focus, so a position
    // read across a focus gap would arrive as one delta covering the gap.
    const bool focused =
        window && glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;

    double x = 0.0;
    double y = 0.0;
    if (focused) glfwGetCursorPos(window, &x, &y);

    const bool canDifference = focused && m_hasPrevCursor;
    const float cursorX =
        canDifference ? static_cast<float>(x - m_prevCursor[0]) : 0.0f;
    const float cursorY =
        canDifference ? static_cast<float>(y - m_prevCursor[1]) : 0.0f;

    m_prevCursor[0] = x;
    m_prevCursor[1] = y;
    m_hasPrevCursor = focused;

    // Muted deltas are dropped rather than held back
    const bool report = !m_muted;
    m_cursorDelta[0] = report ? cursorX : 0.0f;
    m_cursorDelta[1] = report ? cursorY : 0.0f;
    m_scrollDelta[0] = report ? scrollX : 0.0f;
    m_scrollDelta[1] = report ? scrollY : 0.0f;
}

float MouseDevice::getRawInput(InputKey key) const {
    switch (key.kind) {
        case InputKeyKind::MouseButton: {
            if (key.code < 0 || key.code > GLFW_MOUSE_BUTTON_LAST) return 0.0f;
            return glfwGetMouseButton(m_window->getWindow(), key.code) ==
                           GLFW_PRESS
                       ? 1.0f
                       : 0.0f;
        }
        case InputKeyKind::MouseAxis: {
            switch (key.code) {
                case V3D_MOUSE_AXIS_CURSOR_X:
                    return m_cursorDelta[0];
                case V3D_MOUSE_AXIS_CURSOR_Y:
                    return m_cursorDelta[1];
                case V3D_MOUSE_AXIS_SCROLL_X:
                    return m_scrollDelta[0];
                case V3D_MOUSE_AXIS_SCROLL_Y:
                    return m_scrollDelta[1];
                default:
                    return 0.0f;
            }
        }
        case InputKeyKind::Keyboard:
        case InputKeyKind::GamepadButton:
        case InputKeyKind::GamepadAxis:
            break;
    }
    return 0.0f;
}

float MouseDevice::getRawInput(InputAction action) const {
    const std::vector<BoundInput>* inputs = m_profile.getInputs(action);
    if (!inputs) return 0.0f;

    // Largest deflection from zero wins; raw values share no common scale.
    float value = 0.0f;
    for (const BoundInput& input : *inputs) {
        const float raw = getRawInput(input.key);
        if (std::abs(raw) > std::abs(value)) value = raw;
    }
    return value;
}

float MouseDevice::readAxis(BoundInput input) const {
    float value = getRawInput(input.key);

    // Buttons are already 0 or 1; only the sign of the bound range applies.
    if (input.key.kind != InputKeyKind::MouseAxis) {
        return input.range == AxisRange::Negative ? -value : value;
    }

    value *= m_settings.axisSensitivity;
    if (m_settings.invertY && (input.key.code == V3D_MOUSE_AXIS_CURSOR_Y ||
                               input.key.code == V3D_MOUSE_AXIS_SCROLL_Y)) {
        value = -value;
    }

    // A delta has no full-scale value to rescale against, so the deadzone acts
    // as a plain threshold in the axis's own units.
    if (input.range == AxisRange::Full) {
        return applyDeltaThreshold(value, input.deadzone);
    }

    // Each half of an axis is bound separately; the other half reads 0.
    const float magnitude = (input.range == AxisRange::Negative) ? -value : value;
    if (magnitude <= 0.0f) return 0.0f;

    const float processed = applyDeltaThreshold(magnitude, input.deadzone);
    return (input.range == AxisRange::Negative) ? -processed : processed;
}

float MouseDevice::readInput(BoundInput input) const {
    return std::abs(readAxis(input));
}

float MouseDevice::getInput(InputAction action) const {
    const std::vector<BoundInput>* inputs = m_profile.getInputs(action);
    if (!inputs) return 0.0f;

    float value = 0.0f;
    for (const BoundInput& input : *inputs) {
        value = std::max(value, readInput(input));
    }
    return value;
}

float MouseDevice::getAxis(InputAction action) const {
    const std::vector<BoundInput>* inputs = m_profile.getInputs(action);
    if (!inputs) return 0.0f;

    // Bindings sum, so a pair covering opposite halves cancels when both are
    // deflected.
    float value = 0.0f;
    for (const BoundInput& input : *inputs) value += readAxis(input);
    return value;
}

InputKeyResult MouseDevice::getKey(InputKey key) const {
    if (key.kind != InputKeyKind::MouseButton) return IKey_None;
    // Codes overlap between input spaces, so an out-of-range code is a key from
    // another device. glfwGetMouseButton would raise GLFW_INVALID_ENUM.
    if (key.code < 0 || key.code > GLFW_MOUSE_BUTTON_LAST) return IKey_None;
    return glfwGetMouseButton(m_window->getWindow(), key.code) == GLFW_PRESS
               ? IKey_Press
               : IKey_Release;
}

}  // namespace input

}  // namespace v3d
