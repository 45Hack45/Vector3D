#include "input/InputActionRegistry.h"

#include <plog/Log.h>

#include <stdexcept>

#include "input/InputKeys.hpp"

namespace v3d {
namespace input {

InputActionRegistry& InputActionRegistry::instance() {
    static InputActionRegistry instance;
    return instance;
}

InputAction InputActionRegistry::registerAction(std::string name) {
    const InputAction action = makeInputActionID(name);

    auto it = m_actionRegistry.find(action.code);
    if (it != m_actionRegistry.end()) {
        if (it->second == name) return action;

        // A collision would silently make one action drive the other.
        PLOGF << "Input action hash collision: '" << name << "' and '"
              << it->second << "' both hash to " << action.code;
        throw std::logic_error("Input action hash collision between '" + name +
                               "' and '" + it->second + "'");
    }

    m_actionRegistry.emplace(action.code, std::move(name));
    return action;
}

std::optional<InputAction> InputActionRegistry::find(
    std::string_view name) const {
    const InputAction action = makeInputActionID(name);
    auto it = m_actionRegistry.find(action.code);
    if (it == m_actionRegistry.end() || it->second != name) return std::nullopt;
    return action;
}

const std::string* InputActionRegistry::nameOf(InputAction action) const {
    auto it = m_actionRegistry.find(action.code);
    return (it != m_actionRegistry.end()) ? &it->second : nullptr;
}

std::vector<InputActionInfo> InputActionRegistry::getAllInfo() const {
    std::vector<InputActionInfo> out;
    out.reserve(m_actionRegistry.size());
    for (const auto& [code, name] : m_actionRegistry) {
        out.push_back(InputActionInfo{name, InputAction{code}});
    }
    return out;
}

void registerBuiltinActions() {
    InputActionRegistry& registry = InputActionRegistry::instance();
#define V3D_INPUT_ACTION_REGISTER(name) registry.registerAction(#name);
    V3D_INPUT_ACTIONS(V3D_INPUT_ACTION_REGISTER)
#undef V3D_INPUT_ACTION_REGISTER
}

}  // namespace input

}  // namespace v3d
