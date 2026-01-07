#include "ComponentRegistry.h"

#include <iostream>

namespace v3d {
namespace editor {
bool EditorComponentRegistry::registerComponent(
    ComponentEditorRegistrationInfo& info) {
    std::cout << "Registering component '" << info.name << "'\n";

    if (m_componentRegistry.find(info.componentType) !=
        m_componentRegistry.end()) {
        std::cout << "Tried to register the component '" << info.name
                  << "' multiple times\n";
        return false;
    }
    m_componentRegistry[info.componentType] = std::move(info);
    return true;
}

std::vector<std::string> EditorComponentRegistry::getRegisteredNames() const {
    std::vector<std::string> out;
    out.reserve(m_componentRegistry.size());
    for (auto const& kv : m_componentRegistry) out.push_back(kv.second.name);
    return out;
}

const ComponentEditorRegistrationInfo* EditorComponentRegistry::getInfo(
    const std::string& name) const {
    const ComponentEditorRegistrationInfo* componentInfo = nullptr;
    for (auto [componentType, info] : m_componentRegistry) {
        if (info.name == name) {
            // TODO: Change to unique_ptr??? memory leak
            componentInfo = new ComponentEditorRegistrationInfo(info);
            break;
        }
    }
    return componentInfo;
}

const ComponentEditorRegistrationInfo* EditorComponentRegistry::getInfo(
    std::type_index typeIndex) const {
    auto it = m_componentRegistry.find(typeIndex);
    if (it == m_componentRegistry.end()) {
        return nullptr;
    } else {
        auto [componentType, info] = *it;
        return &info;
        // return &(*it).second;
    }
}

std::vector<const ComponentEditorRegistrationInfo*>
EditorComponentRegistry::getAllInfo() const {
    std::vector<const ComponentEditorRegistrationInfo*> componentsInfo;
    componentsInfo.reserve(m_componentRegistry.size());
    for (auto [componentType, info] : m_componentRegistry) {
        // TODO: Change to unique_ptr???
        componentsInfo.push_back(new ComponentEditorRegistrationInfo(info));
    }
    return componentsInfo;
}

}  // namespace editor

}  // namespace v3d
