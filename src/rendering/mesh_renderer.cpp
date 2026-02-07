
#include "mesh_renderer.h"

#include <glm/gtc/quaternion.hpp>

#include "Mesh.h"
#include "engine.h"
#include "rendering/shader.h"
#include "scene.h"
#include "transform.h"
#include "utils/exception.hpp"

namespace v3d {
MeshRenderer::MeshRenderer() {}
MeshRenderer::~MeshRenderer() { resetMesh(); }

// auto MeshRenderer::dependencies() {
//     // return std::tuple<Transform>{};
//     return std::tuple<>{};
// }

void MeshRenderer::init() {
    // Set the rigidbody of the transform to this instance
    auto transform = m_scene->getComponentOfType<Transform>(m_entity);
    assert(transform != nullptr &&
           "Failed to initialize Transform. Missing required component "
           "Transform from the entity");
    m_transform = transform;

    if (m_mesh == nullptr) return;
    // TODO: Maybe load the mesh?
}

void MeshRenderer::registerRenderTarget() {
    m_scene->getEngine()->registerRenderTarget(this);
}

void MeshRenderer::unregisterRenderTarget() {
    if (m_scene != nullptr){
        m_scene->getEngine()->unregisterRenderTarget(this);
    }
}

void MeshRenderer::renderElement() { m_mesh->draw(); }
void MeshRenderer::renderElementInstanced() {
    throw exception::NotImplemented();
}

void MeshRenderer::setUniforms(Shader* shader) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = m_transform->getPos();
    glm::vec3 scale = m_transform->getScale();
    // glm::vec3 axis = m_transform->getRotAxis();
    // float angle = m_transform->getRotAngle();
    // std::cout << "Rotation Axis: (" << axis.x << ", " << axis.y << ", " <<
    // axis.z << ") Angle: " << angle << std::endl;

    glm::vec3 rotAngles = m_transform->getRotationCardanAngles();

    glm::mat4 RotationMatrix = glm::mat4_cast(m_transform->getRotation());

    glm::quat q = glm::quat(rotAngles);  // pitch=x, yaw=y, roll=z

    model = glm::translate(model, pos);

    // model = glm::rotate(model, glm::radians(angle), axis);
    model = model * glm::mat4_cast(q);
    // model = glm::mat4(1.0f);
    // model = RotationMatrix;

    model = glm::scale(model, scale);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    shader->setMat4("model", model);
    shader->setMat3("normalMatrix", normalMatrix);
}
}  // namespace v3d
