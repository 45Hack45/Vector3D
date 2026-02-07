#pragma once

#include <iostream>

#include "Mesh.h"
#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {

class OpenGlBackend : public GraphicsBackend {
   public:
    OpenGlBackend(Window* window);

    Mesh* createMesh(std::string filePath) override;

   protected:
    void initPrimitives() override;
    void frame_update() override;
    void present_frame() override;
    void pre_draw_gizmos_hook() override;
    void post_draw_gizmos_hook() override;

    // Inmediate primitive draw
    virtual void draw_primitive_cube(glm::vec3 position, glm::vec3 scale,
                                     glm::vec4 color, bool wireframe);
    virtual void draw_primitive_sphere(glm::vec3 position, glm::vec3 scale,
                                       glm::vec4 color, bool wireframe);

   private:
    std::vector<MeshOpenGL*> m_meshList;
};
}  // namespace rendering
}  // namespace v3d