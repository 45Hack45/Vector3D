#pragma once

#include "component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace v3d {
class RigidBody;

class Transform : public ComponentBase {
   public:
    Transform() = default;
    ~Transform() override = default;

    // static auto dependencies();
    std::string getComponentName() override { return "Transform"; };

    void drawEditorGUI_Properties() override;

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    glm::vec3 getPos();
    glm::vec3 getScale();
    glm::quat getRotation();
    glm::vec3 getRotationCardanAngles();

    void setScale(const glm::vec3& scale) { m_scale = scale; }
    void setScale(float x, float y, float z) { m_scale = glm::vec3(x, y, z); }

   private:
    Transform* m_parent;
    glm::vec3 m_scale = glm::vec3(1, 1, 1);
    RigidBody* m_rigidBody = nullptr;

    void setParent(Transform* parent);
};
}  // namespace v3d
