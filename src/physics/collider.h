#pragma once

#include <memory>

#include "chrono/core/ChVector3.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemNSC.h"
#include "component.h"
#include "glm/glm.hpp"
#include "physics/rigidbody.h"
#include "scene.h"

namespace v3d {
// class Physics;
class RigidBody;

class ColliderBase : public ComponentBase {
    // friend class Physics;
    friend class RigidBody;

   public:
    // ColliderBase();
    ~ColliderBase() override = default;

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    // //TODO: fuction to edit shape
    // template <typename T, typename... Args>
    // void recreateShape_TODO_CAMBIAR_NOMBRE(Args&&... args);
    // void setShape();

   protected:
    RigidBody* m_rigidBody = nullptr;
    std::shared_ptr<chrono::ChContactMaterialNSC> m_collisionMaterial;
    // std::shared_ptr<T> m_collisionShape;

    virtual std::shared_ptr<chrono::ChCollisionShape> getRawShape() = 0;

    virtual void initColliderProperties() = 0;
};

class ColliderBox : public ColliderBase {
   public:
    void setSize(float x, float y, float z) {
        m_collisionShape.reset();
        m_collisionShape =
            chrono_types::make_shared<chrono::ChCollisionShapeBox>(
                m_collisionMaterial, x, y, z);
    }
    void setSize(glm::vec3& lengths) {
        m_collisionShape.reset();
        m_collisionShape =
            chrono_types::make_shared<chrono::ChCollisionShapeBox>(
                m_collisionMaterial, lengths.x, lengths.y, lengths.z);
    }

   protected:
    std::shared_ptr<chrono::ChCollisionShapeBox> m_collisionShape;

    std::shared_ptr<chrono::ChCollisionShape> getRawShape() override {
        return m_collisionShape;
    }

    void initColliderProperties() override {
        m_collisionShape =
            chrono_types::make_shared<chrono::ChCollisionShapeBox>(
                m_collisionMaterial, 0.1, 0.2, 0.3);
    };
};

// typedef ColliderBase<chrono::ChCollisionShape> Collider;
// typedef ColliderBase<chrono::ChCollisionShapeBox> ColliderBox;
// typedef ColliderBase<chrono::ChCollisionShapeCapsule> ColliderCapsule;
// typedef ColliderBase<chrono::ChCollisionShapeCone> ColliderCone;
// typedef ColliderBase<chrono::ChCollisionShapeConvexHull> ColliderConvexHull;
// typedef ColliderBase<chrono::ChCollisionShapeCylinder> ColliderCylinder;
// typedef ColliderBase<chrono::ChCollisionShapeCylindricalShell>
// ColliderCylindricalShell; typedef
// ColliderBase<chrono::ChCollisionShapeEllipsoid> ColliderEllipsoid; typedef
// ColliderBase<chrono::ChCollisionShapeMeshTriangle> ColliderMeshTriangle;
// typedef ColliderBase<chrono::ChCollisionShapePoint> ColliderPoint;
// typedef ColliderBase<chrono::ChCollisionShapeSphere> ColliderSphere;
// typedef ColliderBase<chrono::ChCollisionShapeTriangle> ColliderTriangle;
// typedef ColliderBase<chrono::ChCollisionShapeArc2D> ColliderArc2D;
// typedef ColliderBase<chrono::ChCollisionShapeBarrel> ColliderBarrel;
// typedef ColliderBase<chrono::ChCollisionShapePath2D> ColliderPath2D;
// typedef ColliderBase<chrono::ChCollisionShapeRoundedBox> ColliderRoundedBox;
// typedef ColliderBase<chrono::ChCollisionShapeRoundedCylinder>
// ColliderRoundedCylinder; typedef
// ColliderBase<chrono::ChCollisionShapeSegment> ColliderSegment; typedef
// ColliderBase<chrono::ChCollisionShapeSegment2D> ColliderSegment2D; typedef
// ColliderBase<chrono::ChCollisionShapeTriangleMesh> ColliderTriangleMesh;

}  // namespace v3d
