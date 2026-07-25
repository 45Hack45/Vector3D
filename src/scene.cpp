
#include "scene.h"

#include <plog/Log.h>

#include "physics/rigidbody.h"
#include "transform.h"

namespace v3d {
entity_ptr Scene::instantiateEntity(std::string name, entity_ptr parent) {
    entity_ptr entity = createEntity();
    entity->m_name = name;
    instantiateEntityComponent<RigidBody>(entity);
    instantiateEntityComponent<Transform>(entity);
    entity->m_transform = getComponentOfType<Transform>(entity.index());
    entity->m_rigidBody = getComponentOfType<RigidBody>(entity.index());
    // setParent wires both the entity hierarchy (m_parent / m_childs) and the
    // physics parent-child constraint (RigidBody::m_parentRelConstrain).
    entity->setParent(parent);
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

void Scene::onLoad() {
    // Pass 1: restore m_scene on every component so init() can use it.
    m_components.for_each([this](ComponentBase& component) {
        component.m_scene = this;
    });

    // Pass 2: init/start each entity's components in insertion order.
    // Entity::m_components stores IDs in the order they were added, so
    // RigidBody always precedes Transform, satisfying dependency ordering.
    for (auto& [id, entity] : m_entities) {
#ifndef NDEBUG
        PLOGV << "[GIZMOS.pass2] visit entity=" << boost::uuids::to_string(id)
              << " name=" << entity.m_name
              << " nComponents=" << entity.m_components.size();
#endif
        for (auto& componentId : entity.m_components) {
            auto* component = m_components.get(componentId);
            if (!component){
#ifndef NDEBUG
                PLOGW << "[GIZMOS.pass2] componentId="
                        << boost::uuids::to_string(componentId)
                        << " -> null (skipped)";
#endif
                continue;                
            }

#ifndef NDEBUG
            assert(component->m_id == componentId &&
                   "onLoad: component id mismatch; archive handle aliasing. "
                   "Corrupt save file; regenerate it.");
#endif
            if (component->m_id != componentId) continue;  // release-build safety net

            component->_init();
            component->init();
            component->start();
        }
    }

    // Pass 3: restore entity cached raw pointers (set during normal
    // instantiateEntity but absent after bare deserialization).
    for (auto& [id, entity] : m_entities) {
        entity.m_transform = getComponentOfType<Transform>(id);
        entity.m_rigidBody = getComponentOfType<RigidBody>(id);
    }

    // Pass 4: re-wire physics parent-child transform constraints.
    // instantiateEntity now always calls Entity::setParent(), so every entity
    // that has a serialized m_parent also has a physics constraint to restore.
    for (auto& [id, entity] : m_entities) {
        if (entity.m_parent && entity.m_transform &&
            entity.m_parent->m_transform) {
            entity_ptr entityPtr(m_entities, id);
            entityPtr->setParent(entity.m_parent);
        }
    }
}
}  // namespace v3d
