
#include "physics/rigidbody.h"

#include "physics/collider.h"
#include "physics/physics.h"
#include "plog/Log.h"
#include "scene.h"

namespace v3d {
void RigidBody::init() {
    // Initialize chrono rigidbody and add to the system
    // TODO: Relate to the parent
    // TODO: Make all entities have entities have rigidbody but with physics disabled, to disable physics set the body to fixed: m_body->SetFixed(true)
    m_body = chrono_types::make_shared<chrono::ChBody>();

    // Initialize
    m_body->SetMass(10);
    m_body->SetInertiaXX(chrono::ChVector3d(4, 4, 4));
    m_body->SetPos(chrono::ChVector3d(0, .1, 0));
    m_body->SetPosDt(chrono::ChVector3d(0, 0, 0));
    // Init acceleration to earth gravity
    m_body->SetPosDt2(chrono::ChVector3d(0, 0.91, 0));

    m_scene->getPhysics()->addBody(*this);
};

void RigidBody::update(double deltaTime) {
    // std::cout << "Pos " << m_scene->getEntity(m_entity)->m_name << ": " << m_body->GetPos() << "\n";
};

void RigidBody::addCollider(ColliderBase &collider) {
    auto shape = collider.getRawShape();
    m_body->AddCollisionShape(shape);
    m_body->EnableCollision(true);
}

}  // namespace v3d
