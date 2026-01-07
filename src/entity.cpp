
#include "entity.h"

#include <plog/Log.h>

#include "component.h"
#include "editor/ComponentRegistry.h"
#include "editor/Editor.h"
#include "misc/cpp/imgui_stdlib.h"
#include "scene.h"
#include "transform.h"

namespace v3d {
const entity_ptr Entity::getPtr() { return m_scene->getEntity(m_id); }

void Entity::drawEditorGUI_Properties() {
    std::string name = getName();

    if (ImGui::InputText("Entity", &name)) {
        setName(name);
    }

    ImGui::Spacing();

    // Draw components
    for (auto component : getComponents()) {
        if (ImGui::CollapsingHeader(component->getComponentName().c_str())) {
            component->drawEditorGUI_Properties();
        }
        ImGui::Spacing();
    }

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup(editor::EditorPopup_PropertiesAddComponent.data(),
                         ImGuiPopupFlags_NoOpenOverExistingPopup);
    }
    if (ImGui::BeginPopup(editor::EditorPopup_PropertiesAddComponent.data())) {
        if (ImGui::MenuItem("Test 1")) {
            std::cout << "aAAAAAAAAAA\n";
            // m_scene->instantiateEntity
        }
        ImGui::MenuItem("Test 2");
        ImGui::MenuItem("Test 3");
        ImGui::MenuItem("Test 4");

        auto componentRegistry = editor::EditorComponentRegistry::instance();

        std::vector<std::string> componentsList =
            componentRegistry.getRegisteredNames();

        for (auto componentName : componentsList) {
            if (ImGui::MenuItem(componentName.c_str())) {
                std::cout << "Instantiating component '" << componentName
                          << "'\n";

                // Create the component
                auto componentInfo = componentRegistry.getInfo(componentName);
                assert(componentInfo && "Component not found in the registry");
                std::unique_ptr<ComponentBase> component =
                    (*componentInfo).factory();

                // Add the component to the scene and entity
                m_scene->insertEntityComponent(m_scene->getEntity(m_id),
                                               std::move(component));
            }
        }

        ImGui::EndPopup();
    }
}

void Entity::setParent(entity_ptr newParent) {
    entity_ptr myPtr = this->getPtr();

    if (m_parent) {
        m_parent->removeChild(myPtr);
    }

    if (!newParent) return;

    newParent->addChild(myPtr);

    m_parent = newParent;

    m_transform->setParent(newParent->m_transform);
}

void Entity::removeChild(entity_ptr child) {
    const auto it = std::find(m_childs.begin(), m_childs.end(), child);
    if (it != m_childs.end()) {
        m_childs.erase(it);
    } else {
        PLOGW << "Child not Found. Failed to remove the entity '"
              << child->getName() << "' from '" << getName() << "' child list";
    }
}

void Entity::addChild(entity_ptr child) {
    const auto it = std::find(m_childs.begin(), m_childs.end(), child);
    if (it != m_childs.end()) {
        PLOGW << "Tried to add a child that already exists";
        return;
    }
    m_childs.push_back(child);
}

std::vector<ComponentBase*> Entity::getComponents() {
    return m_scene->getEntityComponents(getPtr());
}

}  // namespace v3d
