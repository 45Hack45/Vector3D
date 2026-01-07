
#include "transform.h"

#include "physics/rigidbody.h"
#include "scene.h"

namespace v3d {

// auto Transform::dependencies() {
//     return std::tuple<RigidBody>{};
// }

void Transform::init() {
    // Set the rigidbody of the transform to this instance
    auto rigidBody = m_scene->getComponentOfType<RigidBody>(m_entity);
    assert(rigidBody != nullptr &&
           "Failed to initialize Transform. Missing required component "
           "Transform from the entity");
    m_rigidBody = rigidBody;
};

glm::vec3 Transform::getPos() {
    auto pos = m_rigidBody->m_body->GetPos();
    return glm::vec3(pos.x(), pos.y(), pos.z());
}

glm::vec3 Transform::getScale() { return m_scale; }

glm::quat Transform::getRotation() {
    auto quat = m_rigidBody->m_body->GetRot();
    double t = quat.e0() + quat.e1() + quat.e2() + quat.e3();
    return glm::quat(quat.e0(), quat.e1(), quat.e2(), quat.e3());
}

glm::vec3 Transform::getRotationCardanAngles() {
    auto quat = m_rigidBody->m_body->GetRot();
    auto t = quat.GetCardanAnglesXYZ();
    return glm::vec3(t.x(), t.y(), t.z());
}

void Transform::setParent(Transform* parent) {
    m_parent = parent;
    if (parent == nullptr) {
        m_rigidBody->setParent(nullptr);
    } else {
        m_rigidBody->setParent(parent->m_rigidBody);
    }
}

void Transform::drawEditorGUI_Properties() {
    // TODO: !!!!!!!!!!!!!!!!!!!!!!!!! Imgui wraper to convert classes and
    // convinient flags
    glm::vec3 og_position = getPos();
    float position[3] = {og_position.x, og_position.y, og_position.z};
    if (ImGui::InputFloat3("Position", position, "%.3f")) {
        m_rigidBody->setPos(
            chrono::ChVector3(position[0], position[1], position[2]));
    }
}

}  // namespace v3d
