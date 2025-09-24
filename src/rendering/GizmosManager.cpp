
#include "rendering/GizmosManager.h"

#include "GizmosManager.h"
#include "rendering/graphics_backend.h"

void v3d::rendering::GizmosManager::draw_point(glm::vec3 a, float size) {
    m_graphicsBackend->draw_primitive_point(a, size);
}

void v3d::rendering::GizmosManager::draw_line(glm::vec3 a, glm::vec3 b,
                                              float size) {
    m_graphicsBackend->draw_primitive_line(a, b, size);
}
void v3d::rendering::GizmosManager::draw_cube(glm::vec3 position, float size,
                                              glm::vec4 color) {
    m_graphicsBackend->draw_primitive_cube(position, size, color);
}

void v3d::rendering::GizmosManager::draw_sphere(glm::vec3 position, float size,
                                                glm::vec4 color) {
    m_graphicsBackend->draw_primitive_sphere(position, size, color);
}
