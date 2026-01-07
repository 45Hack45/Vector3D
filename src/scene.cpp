
#include "scene.h"

#include <plog/Log.h>

#include "physics/rigidbody.h"
#include "transform.h"

namespace v3d {
entity_ptr Scene::instantiateEntity(std::string name, entity_ptr parent) {
    entity_ptr entity = createEntity(parent);
    entity->m_name = name;
    instantiateEntityComponent<RigidBody>(entity);
    instantiateEntityComponent<Transform>(entity);
    entity->m_transform = getComponentOfType<Transform>(entity.index());
    entity->m_rigidBody = getComponentOfType<RigidBody>(entity.index());
    return entity;
}
entity_ptr Scene::getEntity(entityID_t entityID) {
    if (m_entities.contains(entityID)) {
        return entity_ptr(m_entities, entityID);
    } else {
        PLOGE << "Entity '" << entityID << "' not found";
        return entity_ptr();
    }
}
void Scene::init() {
    // Clear existing entities
    if (m_entities.size()) {
        m_entities.clear();
    }

    m_root = createEntity();
    m_root->m_name = "root";
    instantiateEntityComponent<RigidBody>(m_root);
    instantiateEntityComponent<Transform>(m_root);
    m_root->m_transform = getComponentOfType<Transform>(m_root.index());
    m_root->m_rigidBody = getComponentOfType<RigidBody>(m_root.index());
    m_root->m_rigidBody->setFixed(true);
}
}  // namespace v3d
