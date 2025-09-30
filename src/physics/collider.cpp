
#include "physics/collider.h"

#include "collider.h"
#include "physics/physics.h"

namespace v3d {
void ColliderBase::init() {
    // Set the rigidbody
    auto rigidBody = m_scene->getComponentOfType<RigidBody>(m_entity);
    assert(rigidBody != nullptr &&
           "Failed to initialize Collider. Missing required component "
           "RigidBody from the entity");
    m_rigidBody = rigidBody;

    m_collisionMaterial = m_scene->getPhysics()->getDefaultCollisionMaterial();

    initColliderProperties();

    m_rigidBody->addCollider(*this);
}

void ColliderBox::onDrawGizmos(rendering::GizmosManager* gizmos) {
    // auto hlenghts = m_collisionShape->GetHalflengths();

    // gizmos->draw_cube(m_rigidBody->getPos(), physics::toV3d(hlenghts),
    //                   glm::vec4(1), true);
}
}  // namespace v3d
