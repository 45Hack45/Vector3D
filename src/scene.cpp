
#include "scene.h"

#include "physics/rigidbody.h"
#include "transform.h"

namespace v3d {
entity_ptr Scene::instantiateEntity(std::string name, entity_ptr parent) {
    entity_ptr entity = createEntity(parent);
    entity->m_name = name;
    instantiateEntityComponent<RigidBody>(entity);
    instantiateEntityComponent<Transform>(entity);
    return entity;
}
}  // namespace v3d
