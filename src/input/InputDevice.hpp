#pragma once

#include <boost/serialization/nvp.hpp>
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
/// @brief Which input space a key code belongs to. Codes overlap between spaces
/// (gamepad button 0 and axis 0 are both 0), so a code alone is not a control.
enum class InputKeyKind {
    Keyboard,
    GamepadButton,
    GamepadAxis,
    MouseButton,
    MouseAxis,
};

struct InputKey {
    InputKeyKind kind = InputKeyKind::Keyboard;
    int code = 0;

    constexpr bool operator==(const InputKey& other) const noexcept {
        return kind == other.kind && code == other.code;
    }
    constexpr bool operator!=(const InputKey& other) const noexcept {
        return !(*this == other);
    }
};

/// @brief Portion of an analog axis a binding covers. Ignored for non-axis keys.
/// A half-range binding reads 0 outside its half; Full covers both halves and
/// keeps the sign.
enum class AxisRange {
    Positive,
    Negative,
    Full,
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

/// @brief One resolved control driving an action. range and deadzone apply
/// only when the key is an axis.
struct BoundInput {
    InputKey key;
    AxisRange range = AxisRange::Positive;
    float deadzone = 0.0f;

    constexpr bool operator==(const BoundInput& other) const noexcept {
        return key == other.key && range == other.range;
    }
};

/// @brief Runtime binding table compiled from a persisted DeviceProfile.
class InputProfile {
   public:
    InputProfile() = default;
    ~InputProfile() = default;

    /// @brief Bind an additional control to an action; the strongest bound
    /// control drives it. Binding a key and range that are already bound
    /// updates that binding's deadzone instead of adding a duplicate.
    void bind(InputAction action, BoundInput input) {
        std::vector<BoundInput>& inputs = m_bindings[action];
        for (BoundInput& bound : inputs) {
            // operator== ignores the deadzone, so a match still has to take it.
            if (bound == input) {
                bound.deadzone = input.deadzone;
                return;
            }
        }
        inputs.push_back(input);
    }

    /// @brief Controls bound to an action, or nullptr if unbound.
    const std::vector<BoundInput>* getInputs(InputAction action) const {
        auto it = m_bindings.find(action);
        return (it != m_bindings.end()) ? &it->second : nullptr;
    }

   private:
    std::unordered_map<InputAction, std::vector<BoundInput>, InputActionHasher>
        m_bindings;
};

enum InputDeviceType {
    Undefined,
    Unknown,
    Keyboard,
    Joystick,
    Mouse,
};

/// @brief Per-device tuning, applied on top of whichever profile is active.
struct DeviceSettings {
    float axisSensitivity = 1.0f;  // scales relative axis deltas
    bool invertY = false;          // negates vertical relative axes

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("axisSensitivity", axisSensitivity);
        ar& boost::serialization::make_nvp("invertY", invertY);
    }
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

    /// @brief Value of an action after the device's processing: deadzone
    /// rescaling, trigger remapping, any state update() keeps. Non-negative;
    /// a full-range binding reports its magnitude.
    virtual float getInput(InputAction action) const = 0;
    /// @brief Value of an action as the hardware reports it. For calibration
    /// and binding UI, not for gameplay.
    virtual float getRawInput(InputAction action) const = 0;
    /// @brief Signed value of an action. A full-range binding reports both
    /// halves of its axis; a half-range binding reports its own half carrying
    /// that half's sign. Bindings sum, so opposed ones cancel.
    ///
    /// The result is not confined to [-1, 1]: a relative axis reports the delta
    /// accumulated over one frame, in the axis's own units, which no scale normalises.
    virtual float getAxis(InputAction action) const = 0;

    // Key state
    virtual InputKeyResult getKey(InputKey key) const = 0;

    virtual InputDeviceType getDeviceType() const {
        return InputDeviceType::Undefined;
    }

    /// @brief Stable identity.
    const std::string& getGuid() const { return m_guid; }
    const std::string& getName() const { return m_name; }

    /// @brief Replace the binding table.
    void setProfile(InputProfile profile) { m_profile = std::move(profile); }
    const InputProfile& getProfile() const { return m_profile; }

    /// @brief Replace the tuning applied on top of the bindings.
    void setSettings(DeviceSettings settings) { m_settings = settings; }
    const DeviceSettings& getSettings() const { return m_settings; }

    /// @brief Silence the device.
    void setMuted(bool muted) { m_muted = muted; }
    bool isMuted() const noexcept { return m_muted; }

   protected:
    Window* m_window;
    InputProfile m_profile;
    DeviceSettings m_settings;
    std::string m_guid;
    std::string m_name;
    bool m_muted = false;
};

}  // namespace input

}  // namespace v3d
