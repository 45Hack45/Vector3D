#pragma once

#include <memory>

#include "rendering/GizmosManager.h"

class Shader;

namespace v3d {
namespace rendering {
enum class GraphicsBackendType { NONE, OPENGL_API, VULKAN_API };
enum class WindowBackendHint { NONE, OPENGL_API, VULKAN_API };

class IRenderable {
   public:
    virtual ~IRenderable() = default;
    virtual void renderElement() = 0;
    virtual void renderElementInstanced() = 0;

    // Set uniforms values before rendering
    virtual void setUniforms(Shader* shader) = 0;
};

class IGizmosRenderable {
   public:
    virtual ~IGizmosRenderable() = default;
    virtual void onDrawGizmos(GizmosManager* gizmos) {};
};

struct DrawGizmosSphere : public v3d::rendering::IGizmosRenderable {
    glm::vec3 position = glm::vec3(0.f);
    glm::vec3 scale = glm::vec3(1.f);
    glm::vec4 color = glm::vec4(1.f);
    bool wireframe = false;
    void onDrawGizmos(v3d::rendering::GizmosManager* gizmos) override {
        gizmos->draw_sphere(position, scale, color, wireframe);
    };

    DrawGizmosSphere(glm::vec3 position, glm::vec3 scale = glm::vec3(1.f),
                     glm::vec4 color = glm::vec4(1.f), bool wireframe = false) {
        this->position = position;
        this->scale = scale;
        this->color = color;
        this->wireframe = wireframe;
    }
};

}  // namespace rendering

}  // namespace v3d
