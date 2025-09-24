
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
    // m_rigidBody->m_body->
}
}  // namespace v3d
