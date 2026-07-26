#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

#include "input/InputConfig.hpp"
#include "input/InputDevice.hpp"

namespace v3d {
class Engine;

/// @brief Owns the connected devices and the stored input config.
class InputManager {
    friend class Engine;

   private:
    std::vector<std::unique_ptr<input::InputDevice>> m_devices;
    input::InputConfigStore m_configStore;

    Window* m_window = nullptr;

    bool muted = false;

    void muteInput(bool mute) { muted = mute; }

   public:
    InputManager() = default;
    ~InputManager() = default;

    void setWindow(Window* window) { m_window = window; }

    /// @brief Take ownership of a device and apply any profile saved for its
    /// GUID.
    /// @param device Must have a valid Window.
    void addDevice(std::unique_ptr<input::InputDevice> device);

    /// @brief Reconcile the gamepad list with what is plugged in, then poll
    /// every connected device.
    void update();

    /// @brief Add gamepads that appeared and drop those that went away.
    void refreshGamepads();

    bool isMuted() const noexcept { return muted; }

    float getAction(input::InputAction action) const {
        if (muted) return 0;
        float value = 0.0f;
        for (auto& d : m_devices) {
            value = std::max(value, d->getInput(action));
        }
        return value;  // "OR" behavior, or blend differently
    }

    /// @brief Get a connected device by index, or nullptr if out of range.
    input::InputDevice* getDevice(uint8_t deviceId);

    /// @brief Get the first connected device of a kind, or nullptr.
    input::InputDevice* getDevice(input::InputDeviceType deviceType);

    /// @brief Get a connected device by identity, or nullptr if not connected.
    input::InputDevice* findDeviceByGuid(std::string_view guid);

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
