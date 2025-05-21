#pragma once

#include "component.h"
#include "glm/glm.hpp"

namespace v3d {
class RigidBody;

class Transform : public ComponentBase {
   public:
    Transform() = default;
    ~Transform() override = default;

    // static auto dependencies();

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    glm::vec3 getPos();
    glm::vec3 getRotAxis();
    float getRotAngle();

   private:
    Transform* m_parent;
    RigidBody* m_rigidBody = nullptr;

    void setParent(Transform* parent);
};
}  // namespace v3d
