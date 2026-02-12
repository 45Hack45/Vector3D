#pragma once

#include <chrono/physics/ChLinkTSDA.h>

#include <memory>

#include "chrono/physics/ChLink.h"

namespace v3d {
class Physics;
class RigidBody;

class ConstrainLink {
    friend class Physics;

   private:
   protected:
    std::shared_ptr<chrono::ChLinkBase> m_link = nullptr;
    Physics* m_phSystem = nullptr;

   public:
    ConstrainLink(Physics* system) : m_phSystem(system) {};
    virtual ~ConstrainLink() = default;
};

class ConstraintSpringDamper : public ConstrainLink {
   private:
    chrono::ChLinkTSDA* m_linkSpring;

   public:
    ConstraintSpringDamper() = delete;

    // /// @brief Create and add a spring-damper constraint to the physics
    // system
    // /// @param phSystem
    // /// @param body1 first RigidBody to link
    // /// @param body2 second RigidBody to link
    // /// @param local if true, point locations are relative to the bodies
    // /// @param pos1 point on 1st body
    // /// @param pos2 point on 2nd body
    // ConstraintSpringDamper(Physics* phSystem, RigidBody& body1,
    //                        RigidBody& body2, bool local = true,
    //                        const glm::vec3& pos1 = glm::vec3(0.f),
    //                        const glm::vec3& pos2 = glm::vec3(0.f));

    /// @brief Create and add a spring-damper constraint to the physics system
    /// @param phSystem
    /// @param body1 first ChBody to link
    /// @param body2 second ChBody to link
    /// @param local if true, point locations are relative to the bodies
    /// @param pos1 point on 1st body
    /// @param pos2 point on 2nd body
    ConstraintSpringDamper(Physics* phSystem,
                           std::shared_ptr<chrono::ChBody> body1,
                           std::shared_ptr<chrono::ChBody> body2, bool local,
                           const chrono::ChVector3d pos1,
                           const chrono::ChVector3d pos2);
    /// @brief Remove the constraint from the physics system
    ~ConstraintSpringDamper();

    inline chrono::ChLinkTSDA* getRaw() { return m_linkSpring; }
};

}  // namespace v3d
