#pragma once

#include <string_view>

#include "input/InputKeys.hpp"

namespace v3d {
namespace input {

// GLFW exposes no keyboard identity, so the single system keyboard gets a fixed
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
        if (auto keys = m_profile.getKeys(action)) {
            for (const InputKey& key : *keys) {
                if (glfwGetKey(m_window->getWindow(), key.code) == GLFW_PRESS)
                    return 1.0f;
            }
        }
        return 0.0f;
    }

    float getRawInput(InputAction action) const override {
        // TODO: return the unprocessed value once update() keeps key state.
        return getInput(action);
    }

    InputKeyResult getKey(InputKey key) const override {
        int res = glfwGetKey(m_window->getWindow(), key.code);
        if (res == GLFW_PRESS) {
            return IKey_Press;
        } else {
            return IKey_Release;
        }
    }

    InputDeviceType getDeviceType() const override {
        return input::InputDeviceType::Keyboard;
    }
};
}  // namespace input

}  // namespace v3d
