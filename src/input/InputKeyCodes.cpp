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
};

const std::unordered_map<int, const char*>& codeToName() {
    static const std::unordered_map<int, const char*> map = [] {
        std::unordered_map<int, const char*> m;
        for (const KeyNameEntry& entry : kKeyNames) m[entry.key.code] = entry.name;
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
    auto it = map.find(key.code);
    return (it != map.end()) ? it->second : nullptr;
}

std::optional<InputKey> keyFromName(std::string_view name) {
    const auto& map = nameToKey();
    auto it = map.find(name);
    if (it == map.end()) return std::nullopt;
    return it->second;
}

}  // namespace input

}  // namespace v3d
