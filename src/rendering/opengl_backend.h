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
    void frameUpdate() override;
    void presentFrame() override;
    void preDrawGizmosHook() override;
    void postDrawGizmosHook() override;

    // Primitive draw
    void drawPrimitiveLine(glm::vec3 a, glm::vec3 b, float size, glm::vec4 color) override;
    void drawPrimitiveCube(glm::vec3 position, glm::vec3 scale, glm::vec4 color,
                           bool wireframe) override;
    void drawPrimitiveSphere(glm::vec3 position, glm::vec3 scale,
                             glm::vec4 color, bool wireframe) override;

   private:
    std::vector<MeshOpenGL*> m_meshList;
};
}  // namespace rendering
}  // namespace v3d