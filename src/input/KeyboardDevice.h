#pragma once

#include <string_view>

#include "input/InputKeys.hpp"

namespace v3d {
namespace input {

// GLFW exposes no keyboard identity; the single system keyboard gets a fixed
// GUID, matched by string equality like any joystick GUID.
constexpr std::string_view kKeyboardDeviceGuid = "v3d-keyboard";
constexpr std::string_view kKeyboardDeviceName = "Keyboard";

class KeyboardDevice : public InputDevice {
   public:
    KeyboardDevice(Window* window, InputProfile profile = InputProfile())
        : InputDevice(window, std::string(kKeyboardDeviceGuid),
                      std::string(kKeyboardDeviceName), std::move(profile)) {}

    void update() override {
        // TODO: poll GLFW keys, store current state, for statefull processed
        // input
    }

    float getInput(InputAction action) const override {
        // TODO: apply smoothing and other processing.
        return getRawInput(action);
    }

    float getRawInput(InputAction action) const override {
        if (auto inputs = m_profile.getInputs(action)) {
            for (const BoundInput& input : *inputs) {
                if (input.key.kind != InputKeyKind::Keyboard) continue;
                if (glfwGetKey(m_window->getWindow(), input.key.code) == GLFW_PRESS)
                    return 1.0f;
            }
        }
        return 0.0f;
    }

    InputKeyResult getKey(InputKey key) const override {
        if (key.kind != InputKeyKind::Keyboard) return IKey_None;
        // Codes overlap between input spaces, so an out-of-range code is a key
        // from another device. glfwGetKey would raise GLFW_INVALID_ENUM.
        if (key.code < GLFW_KEY_SPACE || key.code > GLFW_KEY_LAST) {
            return IKey_None;
        }
        return glfwGetKey(m_window->getWindow(), key.code) == GLFW_PRESS
                   ? IKey_Press
                   : IKey_Release;
    }

    InputDeviceType getDeviceType() const override {
        return input::InputDeviceType::Keyboard;
    }
};
}  // namespace input

}  // namespace v3d
