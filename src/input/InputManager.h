#pragma once

#include <fstream>

#include "input/InputDevice.hpp"
#include "input/KeyboardDevice.h"
#include <boost/serialization/unique_ptr.hpp>

namespace v3d {
class Engine;
class InputManager {
    friend class Engine;

   private:
    std::vector<std::unique_ptr<input::InputDevice>> m_devices;
    bool muted = false;

    void muteInput(bool mute) { muted = mute; }

   public:
    InputManager() = default;
    ~InputManager() = default;
    void addDevice(std::unique_ptr<input::InputDevice> device) {
        m_devices.push_back(std::move(device));
    }

    void update() {
        if (muted) return;
        for (auto& d : m_devices) d->update();
    }

    bool isMuted() { return muted; }

    float getAction(input::InputAction action) const {
        if (muted) return 0;
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

    void storeDevice(uint8_t deviceId, std::string filename){
        std::ofstream ofs(filename);
        boost::archive::text_oarchive oa(ofs);
        oa.register_type<v3d::input::KeyboardDevice>();
        oa << m_devices[deviceId];
    }
};
}  // namespace v3d
