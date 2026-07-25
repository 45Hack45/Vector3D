#pragma once

#include <cassert>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils/utils.hpp"
#include "window.h"

namespace v3d {
namespace input {
struct InputKey {
    int code;

    constexpr bool operator==(const InputKey& other) const noexcept {
        return code == other.code;
    }
    constexpr bool operator!=(const InputKey& other) const noexcept {
        return code != other.code;
    }
};

struct InputKeyResult {
    int code;

    constexpr bool operator==(const InputKeyResult& other) const noexcept {
        return code == other.code;
    }
    constexpr bool operator!=(const InputKeyResult& other) const noexcept {
        return code != other.code;
    }
};

struct InputAction {
    uint64_t code;

    constexpr bool operator==(const InputAction& other) const noexcept {
        return code == other.code;
    }
    constexpr bool operator!=(const InputAction& other) const noexcept {
        return code != other.code;
    }
};

struct InputActionHasher {
    size_t operator()(const InputAction& action) const noexcept {
        return std::hash<uint64_t>{}(action.code);
    }
};

inline constexpr InputAction makeInputActionID(std::string_view name) {
    // Generate FNV-1a hash from name
    return InputAction{utils::fnv1a_64(name)};
}

/// @brief Runtime binding table compiled from a persisted DeviceProfile
class InputProfile {
   public:
    InputProfile() = default;
    ~InputProfile() = default;

    /// @brief Bind an additional key to an action; any bound key being down
    /// drives it.
    /// @param key Ignored if already bound to this action.
    void bind(InputAction action, InputKey key) {
        std::vector<InputKey>& keys = m_bindings[action];
        for (const InputKey& bound : keys) {
            if (bound == key) return;
        }
        keys.push_back(key);
    }

    /// @brief Keys bound to an action, or nullptr if unbound.
    const std::vector<InputKey>* getKeys(InputAction action) const {
        auto it = m_bindings.find(action);
        return (it != m_bindings.end()) ? &it->second : nullptr;
    }

   private:
    std::unordered_map<InputAction, std::vector<InputKey>, InputActionHasher>
        m_bindings;
};

enum InputDeviceType {
    Undefined,
    Unknown,
    Keyboard,
    Joystick,
};

/// @brief Base of every physical input device.
class InputDevice {
   public:
    /// @brief Construct a device bound to a window and a stable identity.
    /// @param window Must outlive the device.
    /// @param guid Must survive replug and reboot; saved profiles match on it.
    InputDevice(Window* window, std::string guid, std::string name,
                InputProfile profile = InputProfile())
        : m_window(window),
          m_profile(std::move(profile)),
          m_guid(std::move(guid)),
          m_name(std::move(name)) {
        assert(m_window != nullptr && "InputDevice requires a valid Window");
    };
    virtual ~InputDevice() = default;

    virtual void update() = 0;  // poll the hardware state

    // Action mapped key state
    virtual float getInput(InputAction action) const = 0;
    virtual float getRawInput(InputAction action) const = 0;

    // Key state
    virtual InputKeyResult getKey(InputKey key) const = 0;

    virtual InputDeviceType getDeviceType() const {
        return InputDeviceType::Undefined;
    }

    /// @brief Stable identity.
    const std::string& getGuid() const { return m_guid; }
    /// @brief Human readable device name
    const std::string& getName() const { return m_name; }

    /// @brief Replace the binding table.
    void setProfile(InputProfile profile) { m_profile = std::move(profile); }
    const InputProfile& getProfile() const { return m_profile; }

   protected:
    Window* m_window;
    InputProfile m_profile;
    std::string m_guid;
    std::string m_name;
};

}  // namespace input

}  // namespace v3d
