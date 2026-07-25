#include "input/InputManager.h"

#include <plog/Log.h>

#include <cstdlib>

#include "input/InputActionRegistry.h"
#include "input/InputKeyCodes.h"

namespace v3d {

namespace {

// Build the runtime binding table. Unresolvable entries are skipped here and
// kept in the DeviceProfile.
input::InputProfile compileProfile(const input::DeviceProfile& profile) {
    const input::InputActionRegistry& registry =
        input::InputActionRegistry::instance();
    input::InputProfile compiled;

    for (const input::KeyBinding& binding : profile.bindings) {
        const std::optional<input::InputAction> action =
            registry.find(binding.action);
        if (!action) continue;

        const std::optional<input::InputKey> key =
            input::keyFromName(binding.key);
        if (!key) continue;

        compiled.bind(*action, *key);
    }

    return compiled;
}

// Report what this build cannot resolve. Called when a config enters the store.
void logConfigProblems(const input::DeviceConfig& device) {
    for (const input::DeviceProfile& profile : device.profiles) {
        for (const input::KeyBinding& binding : profile.bindings) {
            switch (input::bindingStatus(binding)) {
                case input::BindingStatus::UnknownAction:
                    PLOGW << "Input config: unknown action '" << binding.action
                          << "' in profile '" << profile.name << "' of device '"
                          << device.guid << "', binding kept unresolved";
                    break;
                case input::BindingStatus::UnknownKey:
                    PLOGW << "Input config: unknown key '" << binding.key
                          << "' in profile '" << profile.name << "' of device '"
                          << device.guid << "', binding kept unresolved";
                    break;
                case input::BindingStatus::Resolved:
                    break;
            }
        }
    }

    if (!device.activeProfile.empty() &&
        !device.findProfile(device.activeProfile)) {
        PLOGW << "Input config: device '" << device.guid
              << "' names missing active profile '" << device.activeProfile
              << "', falling back to the first one";
    }
}

// Make name unique within a device by suffixing a counter.
std::string uniqueProfileName(const input::DeviceConfig& device,
                              std::string name) {
    if (name.empty()) name = std::string(input::kDefaultProfileName);
    if (!device.findProfile(name)) return name;

    for (int suffix = 2;; suffix++) {
        std::string candidate = name + " " + std::to_string(suffix);
        if (!device.findProfile(candidate)) return candidate;
    }
}

}  // namespace

void InputManager::addDevice(std::unique_ptr<input::InputDevice> device) {
    m_devices.push_back(std::move(device));
    applyConfig();
}

input::InputDevice* InputManager::getDevice(uint8_t deviceId) {
    if (deviceId >= m_devices.size()) return nullptr;
    return m_devices[deviceId].get();
}

input::InputDevice* InputManager::getDevice(input::InputDeviceType deviceType) {
    auto it = std::find_if(
        m_devices.begin(), m_devices.end(),
        [deviceType](const std::unique_ptr<input::InputDevice>& device) {
            return device->getDeviceType() == deviceType;
        });
    if (it == m_devices.end()) return nullptr;
    return it->get();
}

input::InputDevice* InputManager::findDeviceByGuid(std::string_view guid) {
    for (auto& device : m_devices) {
        if (device->getGuid() == guid) return device.get();
    }
    return nullptr;
}

input::InputConfigResult InputManager::saveConfig(
    const std::filesystem::path& path) {
    return m_configStore.saveTo(path);
}

input::InputConfigResult InputManager::loadConfig(
    const std::filesystem::path& path) {
    input::InputConfigResult result = m_configStore.loadFrom(path);
    if (result.ok) {
        for (const input::DeviceConfig& config : m_configStore.devices()) {
            logConfigProblems(config);
        }
        applyConfig();
    }
    return result;
}

void InputManager::applyConfig() {
    for (input::DeviceConfig& config : m_configStore.devices()) {
        input::InputDevice* device = findDeviceByGuid(config.guid);
        if (!device) continue;

        // The device is authoritative about its own name and kind. Not a
        // binding change, so it does not by itself make the config dirty.
        config.lastKnownName = device->getName();
        config.deviceKind = input::deviceKindName(device->getDeviceType());

        if (const input::DeviceProfile* active = config.resolvedProfile()) {
            device->setProfile(compileProfile(*active));
        }
    }
}

void InputManager::setDeviceConfig(input::DeviceConfig config) {
    logConfigProblems(config);
    m_configStore.upsertDevice(std::move(config));
    applyConfig();
}

bool InputManager::setActiveProfile(std::string_view guid,
                                    std::string_view profileName) {
    input::DeviceConfig* config = m_configStore.findDevice(guid);
    if (!config || !config->findProfile(profileName)) return false;

    if (config->activeProfile == profileName) return true;

    config->activeProfile = std::string(profileName);
    m_configStore.markDirty(true);
    applyConfig();
    return true;
}

std::string InputManager::duplicateProfile(std::string_view guid,
                                           std::string_view sourceName,
                                           std::string newName) {
    input::DeviceConfig* config = m_configStore.findDevice(guid);
    if (!config) return {};

    const input::DeviceProfile* source = config->findProfile(sourceName);
    if (!source) return {};

    input::DeviceProfile copy = *source;
    copy.name = uniqueProfileName(*config, std::move(newName));

    const std::string storedName = copy.name;
    config->profiles.push_back(std::move(copy));
    m_configStore.markDirty(true);
    applyConfig();
    return storedName;
}

bool InputManager::removeProfile(std::string_view guid,
                                 std::string_view profileName) {
    input::DeviceConfig* config = m_configStore.findDevice(guid);
    if (!config) return false;

    // A device always keeps at least one profile.
    if (config->profiles.size() <= 1) return false;

    auto it = std::find_if(config->profiles.begin(), config->profiles.end(),
                           [profileName](const input::DeviceProfile& profile) {
                               return profile.name == profileName;
                           });
    if (it == config->profiles.end()) return false;

    const bool wasActive = config->activeProfile == profileName;
    config->profiles.erase(it);
    if (wasActive) config->activeProfile = config->profiles.front().name;

    m_configStore.markDirty(true);
    applyConfig();
    return true;
}

std::filesystem::path InputManager::defaultConfigPath() {
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    std::filesystem::path base =
        (appData && *appData) ? std::filesystem::path(appData)
                              : std::filesystem::current_path();
#else
    std::filesystem::path base;
    const char* configHome = std::getenv("XDG_CONFIG_HOME");
    if (configHome && *configHome) {
        base = configHome;
    } else {
        const char* home = std::getenv("HOME");
        base = (home && *home) ? std::filesystem::path(home) / ".config"
                               : std::filesystem::current_path();
    }
#endif
    return base / "Vector3D" / "input_config.xml";
}

}  // namespace v3d
