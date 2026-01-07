#pragma once

#include <memory>

#include "chrono/core/ChVector3.h"
#include "chrono/physics/ChLinkMate.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemSMC.h"
#include "component.h"
#include "physics/ConstrainLink.h"

namespace v3d {
class Transform;
class Physics;
class ColliderBase;
class Vehicle;
class RigidBody;
class ConstrainLink;

class ConstraintParentChild : public ConstrainLink {
   private:
    chrono::ChLinkMateFix* m_linkMateFix;

   public:
    ConstraintParentChild() = delete;
    /// @brief Create and add the constraint to the physics system
    ConstraintParentChild(Physics* phSystem, RigidBody& parent,
                          RigidBody& child);
    /// @brief Remove the constraint from the physics system
    ~ConstraintParentChild();
};

class RigidBody : public ComponentBase {
    friend class Transform;
    friend class Physics;
    friend class Vehicle;
    friend class ConstraintParentChild;

   public:
    RigidBody() = default;
    ~RigidBody() override = default;

    RigidBody(RigidBody&&) = default;
    RigidBody& operator=(RigidBody&&) = default;

    std::string getComponentName() override { return RigidBody::getName(); };
    static std::string getName() { return "RigidBody"; };

    void init() override;
    void start() override {};
    void update(double deltaTime) override;

    void setMass(double mass) { m_body->SetMass(mass); }

    void setInertia(chrono::ChVector3d inertia) {
        m_body->SetInertiaXX(inertia);
    }

    void setPos(glm::vec3 position) {
        m_body->SetPos(chrono::ChVector3d(position.x, position.y, position.z));
    }
    void setPos(chrono::ChVector3d position) { m_body->SetPos(position); }
    void setPos(float x, float y, float z) {
        m_body->SetPos(chrono::ChVector3d(x, y, z));
    }
    glm::vec3 getPos() {
        auto p = m_body->GetPos();
        return glm::vec3(p.x(), p.y(), p.z());
    }

    void setVelocity(chrono::ChVector3d velocity) {
        m_body->SetPosDt(velocity);
    }
    chrono::ChVector3d getVeclocity() { return m_body->GetPosDt(); }

    void setAcceleration(chrono::ChVector3d acceleration) {
        m_body->SetPosDt2(acceleration);
    }
    chrono::ChVector3d getAcceleration() { return m_body->GetPosDt2(); }

    void setFixed(bool fixed) { m_body->SetFixed(fixed); }
    bool isFixed() { return m_body->IsFixed(); }

    void addCollider(ColliderBase& collider);

    void drawEditorGUI_Properties() override;

   private:
    std::shared_ptr<chrono::ChBody> m_body = nullptr;
    RigidBody* m_parent = nullptr;
    std::unique_ptr<ConstraintParentChild> m_parentRelConstrain = nullptr;

    void hardResetBody(chrono::ChBody* newBody);
    void setParent(RigidBody* parent);
};
}  // namespace v3d
