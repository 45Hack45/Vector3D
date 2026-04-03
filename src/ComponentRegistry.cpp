#include "ComponentRegistry.h"

#include <iostream>

namespace v3d {

bool ComponentRegistry::registerComponent(ComponentRegistrationInfo& info) {
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

std::vector<std::string> ComponentRegistry::getRegisteredNames() const {
    std::vector<std::string> out;
    out.reserve(m_componentRegistry.size());
    for (auto const& kv : m_componentRegistry) out.push_back(kv.second.name);
    return out;
}

const ComponentRegistrationInfo* ComponentRegistry::getInfo(
    const std::string& name) const {
    const ComponentRegistrationInfo* componentInfo = nullptr;
    for (auto [componentType, info] : m_componentRegistry) {
        if (info.name == name) {
            // TODO: Change to unique_ptr??? memory leak
            componentInfo = new ComponentRegistrationInfo(info);
            break;
        }
    }
    return componentInfo;
}

const ComponentRegistrationInfo* ComponentRegistry::getInfo(
    std::type_index typeIndex) const {
    auto it = m_componentRegistry.find(typeIndex);
    if (it == m_componentRegistry.end()) {
        return nullptr;
    } else {
        auto [componentType, info] = *it;
        return &info;
    }
}

std::vector<const ComponentRegistrationInfo*>
ComponentRegistry::getAllInfo() const {
    std::vector<const ComponentRegistrationInfo*> componentsInfo;
    componentsInfo.reserve(m_componentRegistry.size());
    for (auto [componentType, info] : m_componentRegistry) {
        // TODO: Change to unique_ptr???
        componentsInfo.push_back(new ComponentRegistrationInfo(info));
    }
    return componentsInfo;
}

}  // namespace v3d
