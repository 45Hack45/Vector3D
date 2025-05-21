#include "physics.h"

#include "physics/collider.h"
#include "physics/rigidbody.h"

namespace v3d {
void Physics::addBody(RigidBody& body) {
    m_system.AddBody(body.m_body);
    m_system.ShowHierarchy(std::cout);
}
}  // namespace v3d
