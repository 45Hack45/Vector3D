#pragma once
#include "chrono/physics/ChSystemNSC.h"

namespace v3d {
class Engine;
class RigidBody;
class ColliderBase;

class Physics {
    friend class Engine;

   public:
    Physics() {
        m_system.SetCollisionSystemType(chrono::ChCollisionSystem::Type::BULLET);
    };
    ~Physics() {};

    void addBody(RigidBody &body);

    std::shared_ptr<chrono::ChContactMaterialNSC> getDefaultCollisionMaterial() {
        return m_defaultCollMat;
    }

   private:
    chrono::ChSystemNSC m_system;
    std::shared_ptr<chrono::ChContactMaterialNSC> m_defaultCollMat = chrono_types::make_shared<chrono::ChContactMaterialNSC>();
};

}  // namespace v3d
