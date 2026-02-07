
#include "physics/rigidbody.h"

#include "editor/ComponentRegistry.h"
#include "entity.h"
#include "physics/collider.h"
#include "physics/physics.h"
#include "plog/Log.h"
#include "rigidbody.h"
#include "scene.h"

namespace v3d {
REGISTER_COMPONENT(RigidBody);

RigidBody::~RigidBody() {
    if (m_body && m_scene) {
        // Ensure that the chBody is cleared.
        m_scene->getPhysics()->removeBody(*this);
    }
}

void RigidBody::init() {
    // Initialize chrono rigidbody and add to the system
    // TODO: Relate to the parent
    // TODO: Make all entities have entities have rigidbody but with physics
    // disabled, to disable physics set the body to fixed:
    // m_body->SetFixed(true)
    m_body = chrono_types::make_shared<chrono::ChBody>();

    // Initialize
    m_body->SetMass(10);
    m_body->SetInertiaXX(chrono::ChVector3d(4, 4, 4));
    m_body->SetPos(chrono::ChVector3d(0, .1, 0));
    m_body->SetPosDt(chrono::ChVector3d(0, 0, 0));
    // Init acceleration to earth gravity
    // m_body->SetPosDt2(chrono::ChVector3d(0, 0.91, 0));

    m_scene->getPhysics()->addBody(*this);
};

void RigidBody::update(double deltaTime) {
    // std::cout << "Pos " << m_scene->getEntity(m_entity)->m_name << ": " <<
    // m_body->GetPos() << "\n";
};

void RigidBody::addCollider(ColliderBase& collider) {
    auto shape = collider.getRawShape();
    m_body->AddCollisionShape(shape);
    m_body->EnableCollision(true);
}

void RigidBody::hardResetBody(std::shared_ptr<chrono::ChBody> newBody) {
    // Remove the current body from the system
    // it will be deleted if it doesn't have external references
    m_scene->getPhysics()->removeBody(*this);
    m_body.reset();

    m_body = newBody;
}

void RigidBody::setParent(RigidBody* parent) {
    // Remove the existing constraint from the physics system
    if (m_parentRelConstrain) m_parentRelConstrain.reset();

    if (parent == nullptr) return;

    // Add the new constraint,
    // this constraint fixes the relative motion of the bodies
    Physics* phsystem = m_scene->getPhysics();
    m_parentRelConstrain =
        std::make_unique<ConstraintParentChild>(phsystem, *parent, *this);
}

void RigidBody::drawEditorGUI_Properties() {
    bool fixed = isFixed();
    if (ImGui::Checkbox("Is Fixed", &fixed)) setFixed(fixed);

    chrono::ChVector3 velocity = getVeclocity();
    chrono::ChVector3 acceleration = getAcceleration();

    float vel[3] = {(float)velocity.x(), (float)velocity.y(),
                    (float)velocity.z()};
    float acc[3] = {(float)acceleration.x(), (float)acceleration.y(),
                    (float)acceleration.z()};
    if (ImGui::DragFloat3("Velocity", vel)) {
        setVelocity(chrono::ChVector3(vel[0], vel[1], vel[2]));
    }
    if (ImGui::DragFloat3("Acceleration", acc)) {
        setAcceleration(chrono::ChVector3(acc[0], acc[1], acc[2]));
    }
}

}  // namespace v3d

v3d::ConstraintParentChild::ConstraintParentChild(Physics* phSystem,
                                                  RigidBody& parent,
                                                  RigidBody& child)
    : ConstrainLink(phSystem) {
    std::shared_ptr<chrono::ChLinkMateFix> link =
        chrono_types::make_shared<chrono::ChLinkMateFix>();
    static int count = 0;
    link->SetName("constraint_parent_child_" + std::to_string(count++));
    link->Initialize(parent.m_body, child.m_body);
    m_link = link;
    m_linkMateFix = link.get();

    m_phSystem->addLink(*this);
}

v3d::ConstraintParentChild::~ConstraintParentChild() {
    m_phSystem->removeLink(*this);
}
