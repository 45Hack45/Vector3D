#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "serialization.hpp"
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

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & code;
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

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & code;
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

struct InputMap {
    InputAction m_action;
    InputKey m_key;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & m_key;
        ar & m_action;
    }
};

class InputProfile {
    friend class boost::serialization::access;

   public:
    InputProfile() = default;
    ~InputProfile() = default;
    void bind(InputAction action, InputKey key) {
        m_mappings[action] = InputMap{action, key};
    }

    const InputMap* getMapping(InputAction action) const {
        auto it = m_mappings.find(action);
        return (it != m_mappings.end()) ? &it->second : nullptr;
    }

   private:
    std::unordered_map<InputAction, InputMap, InputActionHasher> m_mappings;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & m_mappings;
    }
};

enum InputDeviceType {
    Undefined,
    Unknown,
    Keyboard,
    Joystick,
};

class InputDevice {
    friend class boost::serialization::access;

   public:
    InputDevice(Window* window, InputProfile profile)
        : m_window(window), m_profile(std::move(profile)) {};
    virtual ~InputDevice() = default;

    virtual void update() = 0;  // poll the hardware state

    // Action mapped key state
    virtual float getInput(InputAction action) const = 0;
    virtual float getRawInput(InputAction action) const = 0;

    // Key state
    virtual InputKeyResult getKey(InputKey key) const = 0;

    virtual InputDeviceType getDeviceType() {
        return InputDeviceType::Undefined;
    }

   protected:
    Window* m_window;
    InputProfile m_profile;
    bool muted = false;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & m_profile;
        ar & m_profile;
    }
};

}  // namespace input

}  // namespace v3d
