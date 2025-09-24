#pragma once
#include "glm/glm.hpp"

namespace v3d {
namespace rendering {

class GraphicsBackend;

class GizmosManager {
   public:
    GizmosManager(GraphicsBackend* graphicsBackend)
        : m_graphicsBackend(graphicsBackend) {}

    void draw_point(glm::vec3 a, float size = 1);
    void draw_line(glm::vec3 a, glm::vec3 b, float size = 1);
    void draw_cube(glm::vec3 position, float size = 1,
                   glm::vec4 color = glm::vec4(1));
    void draw_sphere(glm::vec3 position, float size = 1,
                     glm::vec4 color = glm::vec4(1));

   private:
    GraphicsBackend* m_graphicsBackend;
};

}  // namespace rendering
}  // namespace v3d