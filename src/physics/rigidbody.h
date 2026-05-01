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
    friend class ConstraintSpringDamper;
    friend class boost::serialization::access;

   public:
    RigidBody() = default;
    ~RigidBody() override;

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

    void hardResetBody(std::shared_ptr<chrono::ChBody> newBody);
    void setParent(RigidBody* parent);

    // Body state persisted across save/load; populated in load(), applied in
    // init() after the ChBody is created.
    struct SavedBodyState {
        double posX = 0, posY = 0.1, posZ = 0;
        double rotE0 = 1, rotE1 = 0, rotE2 = 0, rotE3 = 0;
        double linVelX = 0, linVelY = 0, linVelZ = 0;
        double mass = 10;
        double inertiaX = 4, inertiaY = 4, inertiaZ = 4;
        bool fixed = false;
        bool valid = false;
    };
    SavedBodyState m_savedBodyState;

    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        double posX = 0, posY = 0.1, posZ = 0;
        double rotE0 = 1, rotE1 = 0, rotE2 = 0, rotE3 = 0;
        double linVelX = 0, linVelY = 0, linVelZ = 0;
        double mass = 10;
        double inertiaX = 4, inertiaY = 4, inertiaZ = 4;
        bool fixed = false;
        if (m_body) {
            auto p = m_body->GetPos();
            posX = p.x(); posY = p.y(); posZ = p.z();
            auto r = m_body->GetRot();
            rotE0 = r.e0(); rotE1 = r.e1(); rotE2 = r.e2(); rotE3 = r.e3();
            auto v = m_body->GetPosDt();
            linVelX = v.x(); linVelY = v.y(); linVelZ = v.z();
            mass = m_body->GetMass();
            auto inertia = m_body->GetInertiaXX();
            inertiaX = inertia.x(); inertiaY = inertia.y(); inertiaZ = inertia.z();
            fixed = m_body->IsFixed();
        }
        ar& BOOST_SERIALIZATION_NVP(posX);
        ar& BOOST_SERIALIZATION_NVP(posY);
        ar& BOOST_SERIALIZATION_NVP(posZ);
        ar& BOOST_SERIALIZATION_NVP(rotE0);
        ar& BOOST_SERIALIZATION_NVP(rotE1);
        ar& BOOST_SERIALIZATION_NVP(rotE2);
        ar& BOOST_SERIALIZATION_NVP(rotE3);
        ar& BOOST_SERIALIZATION_NVP(linVelX);
        ar& BOOST_SERIALIZATION_NVP(linVelY);
        ar& BOOST_SERIALIZATION_NVP(linVelZ);
        ar& BOOST_SERIALIZATION_NVP(mass);
        ar& BOOST_SERIALIZATION_NVP(inertiaX);
        ar& BOOST_SERIALIZATION_NVP(inertiaY);
        ar& BOOST_SERIALIZATION_NVP(inertiaZ);
        ar& BOOST_SERIALIZATION_NVP(fixed);
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        double posX, posY, posZ;
        double rotE0, rotE1, rotE2, rotE3;
        double linVelX, linVelY, linVelZ;
        double mass, inertiaX, inertiaY, inertiaZ;
        bool fixed;
        ar& BOOST_SERIALIZATION_NVP(posX);
        ar& BOOST_SERIALIZATION_NVP(posY);
        ar& BOOST_SERIALIZATION_NVP(posZ);
        ar& BOOST_SERIALIZATION_NVP(rotE0);
        ar& BOOST_SERIALIZATION_NVP(rotE1);
        ar& BOOST_SERIALIZATION_NVP(rotE2);
        ar& BOOST_SERIALIZATION_NVP(rotE3);
        ar& BOOST_SERIALIZATION_NVP(linVelX);
        ar& BOOST_SERIALIZATION_NVP(linVelY);
        ar& BOOST_SERIALIZATION_NVP(linVelZ);
        ar& BOOST_SERIALIZATION_NVP(mass);
        ar& BOOST_SERIALIZATION_NVP(inertiaX);
        ar& BOOST_SERIALIZATION_NVP(inertiaY);
        ar& BOOST_SERIALIZATION_NVP(inertiaZ);
        ar& BOOST_SERIALIZATION_NVP(fixed);
        m_savedBodyState = {posX, posY, posZ, rotE0, rotE1, rotE2, rotE3,
                            linVelX, linVelY, linVelZ,
                            mass, inertiaX, inertiaY, inertiaZ, fixed,
                            /*valid=*/true};
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
}  // namespace v3d
