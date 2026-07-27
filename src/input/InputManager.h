#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

#include "input/InputConfig.hpp"
#include "input/InputDevice.hpp"
#include "window.h"

namespace v3d {

/// @brief Owns the connected devices and the stored input config.
class InputManager {
   private:
    std::vector<std::unique_ptr<input::InputDevice>> m_devices;
    input::InputConfigStore m_configStore;

    Window* m_window = nullptr;

    // One bit per InputDeviceType. Applied when an action is read
    uint32_t m_mutedKinds = 0;

    // Whether the startup reconciliation pass has run. Hotplug is event-driven
    // afterwards.
    bool m_gamepadsScanned = false;

    static constexpr uint32_t kindBit(input::InputDeviceType deviceType) {
        return 1u << static_cast<uint32_t>(deviceType);
    }

    void onScroll(double xoffset, double yoffset);

    Window::Subscription m_scrollSub;

    // Apply the hotplug events GLFW recorded since the last update.
    void applyJoystickEvents();

    void addGamepad(int joystickId);

    // Drop the device on a joystick slot, if any. Its DeviceConfig stays in the
    // store, so replugging restores bindings.
    void removeGamepad(int joystickId);

   public:
    InputManager() = default;
    ~InputManager() = default;

    /// @brief Set the window devices poll, and start listening for hotplug.
    /// @param window Must outlive the manager.
    void setWindow(Window* window);

    /// @brief Take ownership of a device and apply any profile saved for its
    /// GUID.
    /// @param device Must have a valid Window.
    void addDevice(std::unique_ptr<input::InputDevice> device);

    /// @brief Apply pending hotplug events, then poll every connected device.
    void update();

    /// @brief Reconcile the gamepad list against every joystick slot GLFW reports
    void refreshGamepads();

    /// @brief Silence reads from one kind of device.
    void setMuted(input::InputDeviceType deviceType, bool mute);

    /// @brief Whether reads from a kind of device return neutral.
    bool isMuted(input::InputDeviceType deviceType) const noexcept {
        return (m_mutedKinds & kindBit(deviceType)) != 0;
    }

    /// @brief Processed value of an action.
    float getAction(input::InputAction action) const {
        float value = 0.0f;
        for (auto& d : m_devices) {
            if (isMuted(d->getDeviceType())) continue;
            value = std::max(value, d->getInput(action));
        }
        return value;  // "OR" behavior, or blend differently
    }

    /// @brief Signed value of an action, summed over the connected devices.
    /// Opposed bindings cancel. A relative axis contributes a per-frame delta
    float getAxis(input::InputAction action) const {
        float value = 0.0f;
        for (const auto& d : m_devices) {
            if (isMuted(d->getDeviceType())) continue;
            value += d->getAxis(action);
        }
        return value;
    }

    /// @brief Value of an action as the hardware reports it
    float getRawAction(input::InputAction action) const;

    /// @brief Key state from one device, or IKey_None if it is not connected or
    /// it is muted.
    input::InputKeyResult getKey(std::string_view guid,
                                 input::InputKey key) const;

    /// @brief Get a connected device by index, or nullptr if out of range.
    input::InputDevice* getDevice(uint8_t deviceId);

    /// @brief Get the first connected device of a kind, or nullptr.
    input::InputDevice* getDevice(input::InputDeviceType deviceType);

    /// @brief Get a connected device by identity, or nullptr if not connected.
    input::InputDevice* findDeviceByGuid(std::string_view guid);
    const input::InputDevice* findDeviceByGuid(std::string_view guid) const;

    inline std::size_t getNumDevices() const noexcept {
        return m_devices.size();
    }

    /// @brief Write the config file, including profiles of devices that are not
    /// currently connected.
    input::InputConfigResult saveConfig(const std::filesystem::path& path);

    /// @brief Read a config file and apply it to the connected devices. On
    /// failure the in-memory config is left untouched.
    input::InputConfigResult loadConfig(const std::filesystem::path& path);

    /// @brief Compile each device's active profile onto the connected devices,
    /// matching by GUID. A device with no stored config keeps its bindings.
    void applyConfig();

    /// @brief Insert or replace the stored config for a device identity and
    /// apply it immediately.
    void setDeviceConfig(input::DeviceConfig config);

    /// @brief Switch which profile a device uses and apply it immediately.
    /// @return false if the device or the profile does not exist.
    bool setActiveProfile(std::string_view guid, std::string_view profileName);

    /// @brief Copy a device profile under a new, unused name. The copy is not
    /// activated.
    /// @param newName Made unique if already taken.
    /// @return Name the copy was stored under, empty if the source was missing.
    std::string duplicateProfile(std::string_view guid,
                                 std::string_view sourceName,
                                 std::string newName);

    /// @brief Delete a profile. Refuses the device's last one; removing the
    /// active profile activates the first remaining.
    /// @return false if it was not found or was the device's only profile.
    bool removeProfile(std::string_view guid, std::string_view profileName);

    const input::InputConfigStore& configStore() const { return m_configStore; }

    /// @brief Whether the config holds changes not yet written to disk.
    bool isConfigDirty() const { return m_configStore.isDirty(); }

    /// @brief Per-user config file location.
    /// @return $XDG_CONFIG_HOME/Vector3D/input_config.xml, or the %APPDATA%
    ///         equivalent on Windows.
    static std::filesystem::path defaultConfigPath();
};
}  // namespace v3d
