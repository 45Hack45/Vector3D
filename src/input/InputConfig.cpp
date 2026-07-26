#include "input/InputConfig.hpp"

#include <plog/Log.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <exception>
#include <fstream>

#include "input/InputActionRegistry.h"
#include "input/InputKeyCodes.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace v3d {
namespace input {

namespace {

// Flush to storage before the rename, so a crash cannot leave an empty config.
bool syncFile(const std::filesystem::path& path) {
#ifdef _WIN32
    int fd = _open(path.string().c_str(), _O_WRONLY | _O_BINARY);
    if (fd < 0) return false;
    const bool ok = _commit(fd) == 0;
    _close(fd);
    return ok;
#else
    int fd = ::open(path.c_str(), O_WRONLY);
    if (fd < 0) return false;
    const bool ok = ::fsync(fd) == 0;
    ::close(fd);
    return ok;
#endif
}

// Persist the rename. Best effort: failure does not fail the save.
void syncDirectory(const std::filesystem::path& path) {
#ifndef _WIN32
    if (path.empty()) return;
    int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) return;
    ::fsync(fd);
    ::close(fd);
#endif
}

}  // namespace

DeviceProfile* DeviceConfig::findProfile(std::string_view name) {
    for (DeviceProfile& profile : profiles) {
        if (profile.name == name) return &profile;
    }
    return nullptr;
}

const DeviceProfile* DeviceConfig::findProfile(std::string_view name) const {
    for (const DeviceProfile& profile : profiles) {
        if (profile.name == name) return &profile;
    }
    return nullptr;
}

DeviceProfile* DeviceConfig::resolvedProfile() {
    if (DeviceProfile* active = findProfile(activeProfile)) return active;
    return profiles.empty() ? nullptr : &profiles.front();
}

const DeviceProfile* DeviceConfig::resolvedProfile() const {
    if (const DeviceProfile* active = findProfile(activeProfile)) return active;
    return profiles.empty() ? nullptr : &profiles.front();
}

DeviceConfig* InputConfigStore::findDevice(std::string_view guid) {
    for (DeviceConfig& device : m_devices) {
        if (device.guid == guid) return &device;
    }
    return nullptr;
}

const DeviceConfig* InputConfigStore::findDevice(std::string_view guid) const {
    for (const DeviceConfig& device : m_devices) {
        if (device.guid == guid) return &device;
    }
    return nullptr;
}

void InputConfigStore::upsertDevice(DeviceConfig config) {
    if (DeviceConfig* existing = findDevice(config.guid)) {
        *existing = std::move(config);
    } else {
        m_devices.push_back(std::move(config));
    }
    m_dirty = true;
}

InputConfigResult InputConfigStore::loadFrom(
    const std::filesystem::path& path) {
    std::ifstream ifs(path);
    if (!ifs) {
        return {false, "Could not open '" + path.string() + "' for reading"};
    }

    std::vector<DeviceConfig> devices;
    try {
        boost::archive::xml_iarchive ia(ifs);

        // Read the version first so an unknown layout is rejected.
        uint32_t formatVersion = 0;
        ia >> boost::serialization::make_nvp("formatVersion", formatVersion);
        if (formatVersion != kInputConfigFormatVersion) {
            return {false, "Config format version " +
                               std::to_string(formatVersion) +
                               " is not supported (this build reads version " +
                               std::to_string(kInputConfigFormatVersion) + ")"};
        }

        ia >> boost::serialization::make_nvp("devices", devices);
    } catch (const std::exception& e) {
        return {false,
                "Failed to parse '" + path.string() + "': " + e.what()};
    }

    std::size_t profileCount = 0;
    for (const DeviceConfig& device : devices) {
        profileCount += device.profiles.size();
    }

    m_devices = std::move(devices);
    m_dirty = false;
    return {true, "Loaded " + std::to_string(profileCount) + " profile(s) for " +
                      std::to_string(m_devices.size()) + " device(s) from " +
                      path.string()};
}

InputConfigResult InputConfigStore::saveTo(const std::filesystem::path& path) {
    std::error_code ec;
    const std::filesystem::path parent = path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent, ec);
        if (ec) {
            return {false, "Could not create '" + parent.string() +
                               "': " + ec.message()};
        }
    }

    std::filesystem::path tempPath = path;
    tempPath += ".tmp";

    std::ofstream ofs(tempPath, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        return {false,
                "Could not open '" + tempPath.string() + "' for writing"};
    }

    try {
        {
            boost::archive::xml_oarchive oa(ofs);
            uint32_t formatVersion = kInputConfigFormatVersion;
            oa << boost::serialization::make_nvp("formatVersion",
                                                 formatVersion);
            oa << boost::serialization::make_nvp("devices", m_devices);
            // The archive flushes and writes closing tags on destruction, it
            // must leave scope before the stream state is checked.
        }
    } catch (const std::exception& e) {
        std::filesystem::remove(tempPath, ec);
        return {false,
                "Failed to write '" + tempPath.string() + "': " + e.what()};
    }

    // Closing an already-failed stream can throw
    try {
        ofs.close();
    } catch (const std::exception&) {
    }

    if (ofs.fail()) {
        std::filesystem::remove(tempPath, ec);
        return {false, "Failed to write '" + tempPath.string() + "'"};
    }

    if (!syncFile(tempPath)) {
        std::filesystem::remove(tempPath, ec);
        return {false, "Could not flush '" + tempPath.string() + "' to disk"};
    }

    std::filesystem::rename(tempPath, path, ec);
    if (ec) {
        std::filesystem::remove(tempPath, ec);
        return {false, "Could not replace '" + path.string() +
                           "': " + ec.message()};
    }
    syncDirectory(parent);

    m_dirty = false;
    std::size_t profileCount = 0;
    for (const DeviceConfig& device : m_devices) {
        profileCount += device.profiles.size();
    }
    return {true, "Saved " + std::to_string(profileCount) + " profile(s) for " +
                      std::to_string(m_devices.size()) + " device(s) to " +
                      path.string()};
}

const char* deviceKindName(InputDeviceType type) {
    switch (type) {
        case InputDeviceType::Keyboard:
            return "keyboard";
        case InputDeviceType::Joystick:
            return "joystick";
        case InputDeviceType::Undefined:
            return "undefined";
        case InputDeviceType::Unknown:
            break;
    }
    return "unknown";
}

DeviceProfile makeDefaultGamepadProfile() {
    const std::string positive = axisDirectionName(AxisDirection::Positive);
    const std::string negative = axisDirectionName(AxisDirection::Negative);

    DeviceProfile profile;
    profile.name = std::string(kDefaultProfileName);
    profile.bindings = {
        {"Accelerate", "GP_AXIS_RIGHT_TRIGGER", positive, kDefaultAxisDeadzone},
        {"Back", "GP_AXIS_LEFT_TRIGGER", positive, kDefaultAxisDeadzone},
        {"Brake", "GP_B", positive, kDefaultAxisDeadzone},
        {"SteerLeft", "GP_AXIS_LEFT_X", negative, kDefaultAxisDeadzone},
        {"SteerRight", "GP_AXIS_LEFT_X", positive, kDefaultAxisDeadzone},
        {"Clutch", "GP_A", positive, kDefaultAxisDeadzone},
    };
    return profile;
}

const char* axisDirectionName(AxisDirection direction) {
    return direction == AxisDirection::Negative ? "negative" : "positive";
}

AxisDirection axisDirectionFromName(std::string_view name) {
    return name == "negative" ? AxisDirection::Negative : AxisDirection::Positive;
}

BindingStatus bindingStatus(const KeyBinding& binding) {
    if (!InputActionRegistry::instance().find(binding.action)) {
        return BindingStatus::UnknownAction;
    }
    if (!keyFromName(binding.key)) return BindingStatus::UnknownKey;
    return BindingStatus::Resolved;
}

}  // namespace input

}  // namespace v3d
