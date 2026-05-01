#pragma once

#include <memory>

#include "chrono/core/ChVector3.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemSMC.h"
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
    friend class boost::serialization::access;

   public:
    // ColliderBase();
    ~ColliderBase() override = default;

    std::string getComponentName() override { return "ColliderBase"; };

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    // //TODO: fuction to edit shape
    // template <typename T, typename... Args>
    // void recreateShape_TODO_CAMBIAR_NOMBRE(Args&&... args);
    // void setShape();

   protected:
    RigidBody* m_rigidBody = nullptr;
    std::shared_ptr<chrono::ChContactMaterialSMC> m_collisionMaterial;
    // std::shared_ptr<T> m_collisionShape;

    virtual std::shared_ptr<chrono::ChCollisionShape> getRawShape() = 0;

    virtual void initColliderProperties() = 0;

    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        // m_rigidBody and m_collisionMaterial are restored in init()
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

class ColliderBox : public ColliderBase {
    friend class boost::serialization::access;

   public:
    std::string getComponentName() override { return "Box Collider"; };
    static std::string getName() { return "ColliderBox"; };

    void setSize(float x, float y, float z) {
        m_sizeX = x; m_sizeY = y; m_sizeZ = z;
        m_collisionShape.reset();
        m_collisionShape =
            chrono_types::make_shared<chrono::ChCollisionShapeBox>(
                m_collisionMaterial, x, y, z);
    }
    void setSize(glm::vec3& lengths) {
        setSize(lengths.x, lengths.y, lengths.z);
    }

   protected:
    std::shared_ptr<chrono::ChCollisionShapeBox> m_collisionShape;

    // Box half-extents; defaults match initColliderProperties() original values.
    float m_sizeX = 0.1f, m_sizeY = 0.2f, m_sizeZ = 0.3f;

    std::shared_ptr<chrono::ChCollisionShape> getRawShape() override {
        return m_collisionShape;
    }

    void initColliderProperties() override {
        m_collisionShape =
            chrono_types::make_shared<chrono::ChCollisionShapeBox>(
                m_collisionMaterial, m_sizeX, m_sizeY, m_sizeZ);
    };

    void onDrawGizmos(rendering::GizmosManager* gizmos);

   private:
    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        ar& boost::serialization::make_nvp("ColliderBase",
                                           boost::serialization::base_object<ColliderBase>(*this));
        ar& BOOST_SERIALIZATION_NVP(m_sizeX);
        ar& BOOST_SERIALIZATION_NVP(m_sizeY);
        ar& BOOST_SERIALIZATION_NVP(m_sizeZ);
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("ColliderBase",
                                           boost::serialization::base_object<ColliderBase>(*this));
        ar& BOOST_SERIALIZATION_NVP(m_sizeX);
        ar& BOOST_SERIALIZATION_NVP(m_sizeY);
        ar& BOOST_SERIALIZATION_NVP(m_sizeZ);
        // initColliderProperties() in init() will use the restored m_sizeX/Y/Z.
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
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
