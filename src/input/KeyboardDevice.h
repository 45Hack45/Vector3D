#pragma once

#include "input/InputKeys.hpp"
#include <boost/serialization/export.hpp>

namespace v3d {
namespace input {
class KeyboardDevice : public InputDevice {
    friend class boost::serialization::access;

   public:
    KeyboardDevice() : InputDevice(nullptr, InputProfile()) {}
    KeyboardDevice(Window* window, InputProfile profile)
        : InputDevice(window, profile) {}
    void update() override {
        // TODO: poll GLFW keys, store current state, for statefull processed
        // input
    }

    float getInput(InputAction action) const override {
        if (muted) return 0;
        if (auto mapping = m_profile.getMapping(action)) {
            return glfwGetKey(m_window->getWindow(), mapping->m_key.code)
                       ? 1.0f
                       : 0.0f;
        }
        return 0.0f;
    }
    float getRawInput(InputAction action) const override {
        if (muted) return 0;
        if (auto mapping = m_profile.getMapping(action)) {
            return glfwGetKey(m_window->getWindow(), mapping->m_key.code)
                       ? 1.0f
                       : 0.0f;
        }
        return 0.0f;
    }

    InputKeyResult getKey(InputKey key) const override {
        if (muted) return IKey_None;
        int res = glfwGetKey(m_window->getWindow(), key.code);
        if (res == GLFW_PRESS) {
            return IKey_Press;
        } else {
            return IKey_Release;
        }
    }

    InputDeviceType getDeviceType() override {
        return input::InputDeviceType::Keyboard;
    }

   private:
    // std::unordered_map<InputKey, InputKeyState> m_keyStates;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        // serialize base class information
        ar& boost::serialization::base_object<InputDevice>(*this);
    }
};
}  // namespace input

}  // namespace v3d
