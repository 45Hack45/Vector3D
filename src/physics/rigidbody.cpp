
#include "physics/rigidbody.h"

#include <algorithm>

#include "ComponentRegistry.h"
#include "entity.h"
#include "physics/collider.h"
#include "physics/physics.h"
#include "plog/Log.h"
#include "rigidbody.h"
#include "scene.h"

Serializable(v3d::RigidBody, "v3d::RigidBody");

namespace v3d {
REGISTER_COMPONENT(RigidBody);

RigidBody::RigidBody(RigidBody&& other) noexcept
    : ComponentBase(std::move(other)),
      m_body(std::move(other.m_body)),
      m_parent(other.m_parent),
      m_parentRelConstrain(std::move(other.m_parentRelConstrain)),
      m_children(std::move(other.m_children)),
      m_savedBodyState(other.m_savedBodyState) {
    other.m_parent = nullptr;
    other.m_children.clear();

    atachConstraintTree(&other);
}

RigidBody& RigidBody::operator=(RigidBody&& other) noexcept {
    if (this == &other) return *this;

    // This body is being overwritten, take it out of its own tree first
    detachConstraintTree();

    ComponentBase::operator=(std::move(other));
    m_body = std::move(other.m_body);
    m_parent = other.m_parent;
    m_parentRelConstrain = std::move(other.m_parentRelConstrain);
    m_children = std::move(other.m_children);
    m_savedBodyState = other.m_savedBodyState;

    other.m_parent = nullptr;
    other.m_children.clear();

    atachConstraintTree(&other);
    return *this;
}

RigidBody::~RigidBody() {
    detachConstraintTree();

    if (m_body && m_scene) {
        // Ensure that the chBody is cleared.
        m_scene->getPhysics()->removeBody(*this);
    }
}

void RigidBody::init() {
    // Initialize chrono rigidbody and add to the system
    // TODO: Relate to the parent
    // TODO: Make all entities have rigidbody but with physics disabled;
    // to disable physics set the body to fixed: m_body->SetFixed(true)
    m_body = chrono_types::make_shared<chrono::ChBody>();

    if (m_savedBodyState.valid) {
        // Restore persisted body state from the last save.
        m_body->SetPos(chrono::ChVector3d(m_savedBodyState.posX,
                                          m_savedBodyState.posY,
                                          m_savedBodyState.posZ));
        m_body->SetRot(chrono::ChQuaterniond(m_savedBodyState.rotE0,
                                              m_savedBodyState.rotE1,
                                              m_savedBodyState.rotE2,
                                              m_savedBodyState.rotE3));
        m_body->SetPosDt(chrono::ChVector3d(m_savedBodyState.linVelX,
                                             m_savedBodyState.linVelY,
                                             m_savedBodyState.linVelZ));
        m_body->SetMass(m_savedBodyState.mass);
        m_body->SetInertiaXX(chrono::ChVector3d(m_savedBodyState.inertiaX,
                                                 m_savedBodyState.inertiaY,
                                                 m_savedBodyState.inertiaZ));
        m_body->SetFixed(m_savedBodyState.fixed);
    } else {
        // Default state for newly created bodies.
        m_body->SetMass(10);
        m_body->SetInertiaXX(chrono::ChVector3d(4, 4, 4));
        m_body->SetPos(chrono::ChVector3d(0, .1, 0));
        m_body->SetPosDt(chrono::ChVector3d(0, 0, 0));
    }

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

    // The old body is gone, rebuild every constraint that referenced it
    rebuildParentConstraint();
    for (auto* child : m_children) {
        if (child) child->rebuildParentConstraint();
    }
}

void RigidBody::setParent(RigidBody* parent) {
    // Stop the previous parent from tracking this body
    if (m_parent) m_parent->removeChildBody(this);

    m_parent = parent;

    // Let the new parent track this body, so it can rebuild the constraint if
    // its own body is replaced
    if (m_parent) m_parent->addChildBody(this);

    rebuildParentConstraint();
}

void RigidBody::rebuildParentConstraint() {
    // Remove the existing constraint from the physics system
    if (m_parentRelConstrain) m_parentRelConstrain.reset();

    if (!parentConstraintAllowed()) return;

    // Add the new constraint,
    // this constraint fixes the relative motion of the bodies
    Physics* phsystem = m_scene->getPhysics();
    m_parentRelConstrain =
        std::make_unique<ConstraintParentChild>(phsystem, *m_parent, *this);
}

bool RigidBody::parentConstraintAllowed() const {
    if (m_parent == nullptr) return false;
    if (!m_body || !m_parent->m_body) return false;

    // The root is a hierarchy anchor and a fixed body. Constraining to it would
    // weld the entity in place and remove it from the simulation
    if (m_parent->isSceneRoot()) return false;

    return true;
}

bool RigidBody::isSceneRoot() const {
    if (m_scene == nullptr) return false;
    return m_entity == m_scene->getRootId();
}

void RigidBody::addChildBody(RigidBody* child) {
    if (child == nullptr) return;
    const auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) return;
    m_children.push_back(child);
}

void RigidBody::removeChildBody(RigidBody* child) {
    const auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) m_children.erase(it);
}

void RigidBody::detachConstraintTree() {
    // Stop the parent from tracking this body
    if (m_parent) {
        m_parent->removeChildBody(this);
        m_parent = nullptr;
    }

    // Clear the constraints of the children, they reference this body.
    // Done here because component destruction order is not defined
    for (auto* child : m_children) {
        if (!child) continue;
        child->m_parentRelConstrain.reset();
        child->m_parent = nullptr;
    }
    m_children.clear();
}

void RigidBody::atachConstraintTree(RigidBody* previous) {
    // The chrono bodies moved with us, only the raw back pointers of the
    // neighbours still hold the old address
    if (m_parent) {
        std::replace(m_parent->m_children.begin(), m_parent->m_children.end(),
                     previous, this);
    }
    for (auto* child : m_children) {
        if (child && child->m_parent == previous) child->m_parent = this;
    }
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
