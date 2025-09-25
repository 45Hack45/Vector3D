#pragma once

#include "input/InputDevice.hpp"

namespace v3d {
class InputManager {
   private:
    std::vector<std::unique_ptr<input::InputDevice>> m_devices;

   public:
    InputManager() = default;
    ~InputManager() = default;
    void addDevice(std::unique_ptr<input::InputDevice> device) {
        m_devices.push_back(std::move(device));
    }

    void update() {
        for (auto& d : m_devices) d->update();
    }

    float getAction(input::InputAction action) const {
        float value = 0.0f;
        for (auto& d : m_devices) {
            value = std::max(value, d->getInput(action));
        }
        return value;  // "OR" behavior, or blend differently
    }

    input::InputDevice* getDevice(uint8_t deviceId) {
        return m_devices[deviceId].get();
    }
    input::InputDevice* getDevice(input::InputDeviceType deviceType) {
        auto it = std::find_if(
            m_devices.begin(), m_devices.end(),
            [deviceType](const std::unique_ptr<input::InputDevice>& device) {
                return device->getDeviceType() == deviceType;
            });
        // Dereference iterator and get a pointer
        return (*it).get();
    }

    inline std::size_t getNumDevices() const noexcept {
        return m_devices.size();
    }
};
}  // namespace v3d
