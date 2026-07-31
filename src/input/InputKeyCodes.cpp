#include "input/InputKeyCodes.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>

namespace v3d {
namespace input {

namespace {

struct KeyNameEntry {
    const char* name;
    InputKey key;
};

constexpr KeyNameEntry kKeyNames[] = {
#define V3D_INPUT_KEY_NAME_ENTRY(name) {#name, key::IK_##name},
    V3D_INPUT_KEYS(V3D_INPUT_KEY_NAME_ENTRY)
#undef V3D_INPUT_KEY_NAME_ENTRY

#define V3D_GAMEPAD_BUTTON_NAME_ENTRY(name) {"GP_" #name, gamepad::IB_##name},
    V3D_GAMEPAD_BUTTONS(V3D_GAMEPAD_BUTTON_NAME_ENTRY)
#undef V3D_GAMEPAD_BUTTON_NAME_ENTRY

#define V3D_GAMEPAD_AXIS_NAME_ENTRY(name) {"GP_AXIS_" #name, gamepad::IA_##name},
    V3D_GAMEPAD_AXES(V3D_GAMEPAD_AXIS_NAME_ENTRY)
#undef V3D_GAMEPAD_AXIS_NAME_ENTRY

#define V3D_MOUSE_BUTTON_NAME_ENTRY(name) {"MOUSE_" #name, mouse::IB_##name},
    V3D_MOUSE_BUTTONS(V3D_MOUSE_BUTTON_NAME_ENTRY)
#undef V3D_MOUSE_BUTTON_NAME_ENTRY

#define V3D_MOUSE_AXIS_NAME_ENTRY(name) {"MOUSE_AXIS_" #name, mouse::IA_##name},
    V3D_MOUSE_AXES(V3D_MOUSE_AXIS_NAME_ENTRY)
#undef V3D_MOUSE_AXIS_NAME_ENTRY
};

// Codes repeat across kinds, so the reverse table is keyed on the pair.
uint64_t packKey(InputKey key) {
    return (static_cast<uint64_t>(key.kind) << 32) |
           static_cast<uint32_t>(key.code);
}

const std::unordered_map<uint64_t, const char*>& codeToName() {
    static const std::unordered_map<uint64_t, const char*> map = [] {
        std::unordered_map<uint64_t, const char*> m;
        for (const KeyNameEntry& entry : kKeyNames) m[packKey(entry.key)] = entry.name;
        return m;
    }();
    return map;
}

const std::unordered_map<std::string_view, InputKey>& nameToKey() {
    static const std::unordered_map<std::string_view, InputKey> map = [] {
        std::unordered_map<std::string_view, InputKey> m;
        for (const KeyNameEntry& entry : kKeyNames) m[entry.name] = entry.key;
        return m;
    }();
    return map;
}

}  // namespace

const char* keyName(InputKey key) {
    const auto& map = codeToName();
    auto it = map.find(packKey(key));
    return (it != map.end()) ? it->second : nullptr;
}

std::optional<InputKey> keyFromName(std::string_view name) {
    const auto& map = nameToKey();
    auto it = map.find(name);
    if (it == map.end()) return std::nullopt;
    return it->second;
}

bool isTriggerAxis(InputKey key) {
    return key.kind == InputKeyKind::GamepadAxis &&
           (key.code == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER ||
            key.code == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER);
}

bool isRelativeAxis(InputKey key) {
    return key.kind == InputKeyKind::MouseAxis;
}

float applyDeadzone(float value, float deadzone) {
    deadzone = std::clamp(deadzone, 0.0f, 0.99f);
    if (value <= deadzone) return 0.0f;
    return (value - deadzone) / (1.0f - deadzone);
}

float applyDeadzoneSigned(float value, float deadzone) {
    return std::copysign(applyDeadzone(std::abs(value), deadzone), value);
}

float applyDeltaThreshold(float value, float threshold) {
    return std::abs(value) <= std::abs(threshold) ? 0.0f : value;
}

}  // namespace input

}  // namespace v3d
