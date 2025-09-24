#pragma once

#include "mesh.h"
#include "rendering/graphics_backend.h"
#include <iostream>

namespace v3d {
namespace rendering {

class OpenGlBackend : public GraphicsBackend {
   public:
    OpenGlBackend(Window* window) : GraphicsBackend(window) {}

    void init() override;
    void cleanup() override;

    Mesh* createMesh(std::string filePath) override;

   protected:
    void initPrimitives() override;
    void frame_update() override;
    void present_frame() override;
    void pre_draw_gizmos_hook() override;
    void post_draw_gizmos_hook() override;

    // Inmediate primitive draw
    virtual void draw_primitive_cube(glm::vec3 position, float size, glm::vec4 color);
    virtual void draw_primitive_sphere(glm::vec3 position, float size, glm::vec4 color);
   private:
    std::vector<MeshOpenGL*> m_meshList;
};
}  // namespace rendering
}  // namespace v3d