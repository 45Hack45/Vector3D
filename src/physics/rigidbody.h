#pragma once

#include <memory>

#include "chrono/core/ChVector3.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemNSC.h"
#include "component.h"

namespace v3d {
class Transform;
class Physics;
class ColliderBase;
class Vehicle;

class RigidBody : public ComponentBase {
    friend class Transform;
    friend class Physics;
    friend class Vehicle;

   public:
    RigidBody() = default;
    ~RigidBody() override = default;

    void init() override;
    void start() override {};
    void update(double deltaTime) override;

    void setMass(double mass) { m_body->SetMass(mass); }

    void setInertia(chrono::ChVector3d inertia) {
        m_body->SetInertiaXX(inertia);
    }

    void setPos(chrono::ChVector3d position) { m_body->SetPos(position); }
    void setPos(float x, float y, float z) {
        m_body->SetPos(chrono::ChVector3d(x, y, z));
    }

    void setVelocity(chrono::ChVector3d velocity) {
        m_body->SetPosDt(velocity);
    }

    void setAcceleration(chrono::ChVector3d acceleration) {
        m_body->SetPosDt2(acceleration);
    }

    void setFixed(bool fixed) { m_body->SetFixed(fixed); }

    void addCollider(ColliderBase &collider);

   private:
    std::shared_ptr<chrono::ChBody> m_body;

    void hardResetBody(chrono::ChBody *newBody);
};

}  // namespace v3d
