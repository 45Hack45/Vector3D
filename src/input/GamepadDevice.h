#pragma once

#include <string>

#include "input/InputKeys.hpp"

namespace v3d {
namespace input {

/// @brief A controller read through the GLFW gamepad mapping. Only joysticks
/// glfwJoystickIsGamepad() accepts.
class GamepadDevice : public InputDevice {
   public:
    /// @brief Construct for a connected joystick id.
    /// @param joystickId GLFW_JOYSTICK_* id; must currently be a mapped gamepad.
    GamepadDevice(Window* window, int joystickId, std::string guid,
                  std::string name, InputProfile profile = InputProfile());

    /// @brief Poll the gamepad. A device unplugged mid-frame reports neutral
    /// until InputManager notices and removes it.
    void update() override;

    /// @brief Value of one control with no processing applied
    float getRawInput(InputKey key) const;
    /// @brief Strongest unprocessed value among an action's bindings, by
    /// distance from zero. An action bound to a trigger reports -1 while
    /// released.
    float getRawInput(InputAction action) const override;

    /// @brief Signed value of one binding, with trigger rescaling and its
    /// deadzone applied, over [-1, 1]. The half of an axis the binding does not
    /// cover reads 0.
    float readAxis(BoundInput input) const;

    /// @brief Magnitude of one binding, over [0, 1].
    float readInput(BoundInput input) const;

    float getInput(InputAction action) const override;

    float getAxis(InputAction action) const override;

    InputKeyResult getKey(InputKey key) const override;

    InputDeviceType getDeviceType() const override {
        return InputDeviceType::Joystick;
    }

    /// @brief GLFW joystick id. Not stable across replug, unlike getGuid().
    int getJoystickId() const { return m_joystickId; }

   private:
    int m_joystickId;
    GLFWgamepadstate m_state{};
};

}  // namespace input

}  // namespace v3d
