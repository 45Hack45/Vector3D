#pragma once

#include "component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace v3d {
class RigidBody;

class Transform : public ComponentBase {
    friend class Entity;
    friend class boost::serialization::access;

   public:
    Transform() = default;
    ~Transform() override = default;

    // static auto dependencies();
    std::string getComponentName() override { return "Transform"; };
    static std::string getName() { return "Transform"; };

    void drawEditorGUI_Properties() override;

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    glm::vec3 getPos();
    glm::vec3 getScale();
    glm::quat getRotation();
    glm::vec3 getRotationCardanAngles();

    void setScale(const glm::vec3& scale) { m_scale = scale; }
    void setScale(float x, float y, float z) { m_scale = glm::vec3(x, y, z); }

   private:
    Transform* m_parent = nullptr;
    glm::vec3 m_scale = glm::vec3(1, 1, 1);
    RigidBody* m_rigidBody = nullptr;

    void setParent(Transform* parent);

    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        float scale[3]{m_scale.x, m_scale.y, m_scale.z};
        ar& BOOST_SERIALIZATION_NVP(scale);
        // m_parent and m_rigidBody are raw pointers restored in init()
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        float scale[3];
        ar& BOOST_SERIALIZATION_NVP(scale);
        m_scale = glm::vec3(scale[0], scale[1], scale[2]);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
}  // namespace v3d
