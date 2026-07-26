#include "input/InputKeyCodes.h"

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

}  // namespace input

}  // namespace v3d
