#pragma once

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "input/InputDevice.hpp"

namespace v3d {
namespace input {

// Bump when the layout changes incompatibly.
constexpr uint32_t kInputConfigFormatVersion = 1;

constexpr std::string_view kDefaultProfileName = "Default";

// Applied to axis bindings that do not say otherwise.
constexpr float kDefaultAxisDeadzone = 0.15f;

/// @brief Outcome of a config save or load.
struct InputConfigResult {
    bool ok = false;
    std::string message;
};

/// @brief Whether a binding resolves against the registry.
enum class BindingStatus {
    Resolved,
    UnknownAction,
    UnknownKey,
};

/// @brief Key binding. direction and deadzone describe the axis half this
/// binding reads and are ignored when the key is not an axis.
struct KeyBinding {
    std::string action;     // action registry name, e.g. "Accelerate"
    std::string key;        // key table name, e.g. "SPACE"
    std::string direction;  // "positive" or "negative"
    float deadzone = kDefaultAxisDeadzone;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("action", action);
        ar& boost::serialization::make_nvp("key", key);
        ar& boost::serialization::make_nvp("direction", direction);
        ar& boost::serialization::make_nvp("deadzone", deadzone);
    }
};

/// @brief Persisted name of an axis direction.
const char* axisDirectionName(AxisDirection direction);

/// @brief Axis direction for a persisted name. Anything unrecognised, including
/// the empty string a button binding carries, reads as positive.
AxisDirection axisDirectionFromName(std::string_view name);

/// @brief Named set of key bindings.
struct DeviceProfile {
    std::string name;  // "Default", "Racing", ...
    std::vector<KeyBinding> bindings;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("name", name);
        ar& boost::serialization::make_nvp("bindings", bindings);
    }
};

/// @brief Device configuration, profiles and key bindings.
struct DeviceConfig {
    std::string deviceKind;     // "keyboard", "joystick", ...
    std::string guid;           // matched against InputDevice::getGuid()
    std::string lastKnownName;  // display only, refreshed when the device connects
    std::string activeProfile;  // name of the profile in use
    std::vector<DeviceProfile> profiles;

    /// @brief Find one of this device's profiles by name, or nullptr.
    DeviceProfile* findProfile(std::string_view name);
    const DeviceProfile* findProfile(std::string_view name) const;

    /// @brief Active profile, falling back to the first.
    /// nullptr if the device has no profiles.
    DeviceProfile* resolvedProfile();
    const DeviceProfile* resolvedProfile() const;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("deviceKind", deviceKind);
        ar& boost::serialization::make_nvp("guid", guid);
        ar& boost::serialization::make_nvp("lastKnownName", lastKnownName);
        ar& boost::serialization::make_nvp("activeProfile", activeProfile);
        ar& boost::serialization::make_nvp("profiles", profiles);
    }
};

/// @brief Source of truth for devices and key bindings.
class InputConfigStore {
   public:
    InputConfigStore() = default;
    ~InputConfigStore() = default;

    const std::vector<DeviceConfig>& devices() const { return m_devices; }
    std::vector<DeviceConfig>& devices() { return m_devices; }

    /// @brief Find the config stored for a device identity, or nullptr.
    DeviceConfig* findDevice(std::string_view guid);
    const DeviceConfig* findDevice(std::string_view guid) const;

    /// @brief Insert a device config, or replace the one with the same GUID.
    void upsertDevice(DeviceConfig config);

    /// @brief Read the config file at path, replacing the store's contents. The
    /// store is left untouched unless the load fully succeeded.
    InputConfigResult loadFrom(const std::filesystem::path& path);

    /// @brief Write the store to path, including devices that are not
    /// currently connected. Parent directories are created.
    InputConfigResult saveTo(const std::filesystem::path& path);

    /// @brief Whether the store holds changes not written to disk.
    bool isDirty() const { return m_dirty; }
    void markDirty(bool dirty) { m_dirty = dirty; }

   private:
    std::vector<DeviceConfig> m_devices;
    bool m_dirty = false;
};

/// @brief Persisted name of a device kind, e.g. "keyboard".
const char* deviceKindName(InputDeviceType type);

/// @brief Built-in bindings for a gamepad with nothing saved for its GUID.
DeviceProfile makeDefaultGamepadProfile();

/// @brief Whether a binding resolves against the registry.
BindingStatus bindingStatus(const KeyBinding& binding);

}  // namespace input

}  // namespace v3d
