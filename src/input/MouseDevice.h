#pragma once

#include <string>
#include <string_view>

#include "input/InputKeys.hpp"

namespace v3d {
namespace input {

// GLFW exposes no mouse identity; the single system mouse gets a fixed GUID,
// matched by string equality like any joystick GUID.
constexpr std::string_view kMouseDeviceGuid = "v3d-mouse";
constexpr std::string_view kMouseDeviceName = "Mouse";

/// @brief The system mouse: buttons, cursor movement and scroll wheel. Its axes
/// are per-frame deltas rather than positions, so their values are unbounded
/// and carry no normalised full scale.
class MouseDevice : public InputDevice {
   public:
    MouseDevice(Window* window, InputProfile profile = InputProfile());

    /// @brief Record one scroll event.
    void accumulateScroll(float xoffset, float yoffset);

    /// @brief Sample the cursor and drain the scroll accumulator
    void update() override;

    /// @brief Value of one control with no processing applied. An axis reports
    /// this frame's delta, in pixels or wheel steps, before sensitivity.
    float getRawInput(InputKey key) const;
    float getRawInput(InputAction action) const override;

    /// @brief Signed value of one binding, with sensitivity, Y inversion and
    /// the binding's deadzone applied. The half of an axis the binding does not
    /// cover reads 0.
    float readAxis(BoundInput input) const;

    /// @brief Magnitude of one binding.
    float readInput(BoundInput input) const;

    float getInput(InputAction action) const override;

    float getAxis(InputAction action) const override;

    InputKeyResult getKey(InputKey key) const override;

    InputDeviceType getDeviceType() const override {
        return InputDeviceType::Mouse;
    }

    /// @brief Cursor movement over the last update, in pixels, positive right
    /// and down.
    float getCursorDeltaX() const { return m_cursorDelta[0]; }
    float getCursorDeltaY() const { return m_cursorDelta[1]; }

    /// @brief Wheel movement over the last update, in wheel steps.
    float getScrollDeltaX() const { return m_scrollDelta[0]; }
    float getScrollDeltaY() const { return m_scrollDelta[1]; }

    // TODO: absolute cursor position, for UI picking and gizmo dragging.
    // TODO: cursor mode, so mouse-look can hold GLFW_CURSOR_DISABLED

   private:
    // Written by the GLFW scroll callback between updates.
    float m_scrollAccum[2]{};

    float m_cursorDelta[2]{};
    float m_scrollDelta[2]{};

    double m_prevCursor[2]{};
    // False until a cursor sample exists that the next one can be differenced
    // against.
    bool m_hasPrevCursor = false;
};

}  // namespace input

}  // namespace v3d
