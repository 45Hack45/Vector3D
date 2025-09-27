
#include "entity.h"

#include "Editor.h"
#include "scene.h"

namespace v3d {
const entity_ptr Entity::getPtr() { return m_scene->getEntity(m_id); }

void Entity::drawEditorGUI_Properties() {
    editor::Editor::Instance()->renderEntityEditorPropertiesGui(this);
}

std::vector<ComponentBase *> Entity::getComponents() {
    return m_scene->getEntityComponents(getPtr());
}

}  // namespace v3d
