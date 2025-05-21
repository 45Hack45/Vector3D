
#include "physics/collider.h"

#include "physics/physics.h"

namespace v3d {
void ColliderBase::init() {
    // Set the rigidbody
    auto rigidBody = m_scene->getComponentOfType<RigidBody>(m_entity);
    assert(rigidBody != nullptr && "Failed to initialize Transform. Missing required component Transform from the entity");
    m_rigidBody = rigidBody;

    m_collisionMaterial = m_scene->getPhysics()->getDefaultCollisionMaterial();

    initColliderProperties();

    m_rigidBody->addCollider(*this);
}
}  // namespace v3d
