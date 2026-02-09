
#include "rendering/GizmosManager.h"

#include "GizmosManager.h"
#include "rendering/graphics_backend.h"

void v3d::rendering::GizmosManager::draw_point(glm::vec3 a, float size, glm::vec4 color) {
    m_graphicsBackend->drawPrimitivePoint(a, size, color);
}

void v3d::rendering::GizmosManager::draw_line(glm::vec3 a, glm::vec3 b,
                                              float size, glm::vec4 color) {
    m_graphicsBackend->drawPrimitiveLine(a, b, size, color);
}
void v3d::rendering::GizmosManager::draw_cube(glm::vec3 position,
                                              glm::vec3 scale, glm::vec4 color,
                                              bool wireframe) {
    m_graphicsBackend->drawPrimitiveCube(position, scale, color, wireframe);
}

void v3d::rendering::GizmosManager::draw_sphere(glm::vec3 position,
                                                glm::vec3 scale,
                                                glm::vec4 color,
                                                bool wireframe) {
    m_graphicsBackend->drawPrimitiveSphere(position, scale, color, wireframe);
}
