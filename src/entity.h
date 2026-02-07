#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "DefinitionCore.hpp"
// #include "utils/vector_ptr.hpp"

namespace v3d {
class Scene;
class Transform;
class RigidBody;

class Entity : public IEditorGUISelectable {
    friend class Scene;

   public:
    std::string getName() const { return m_name; }
    void setName(std::string name) { m_name = name; }

    const std::vector<entity_ptr>& getChilds() const { return m_childs; }

    inline const entity_ptr getPtr();

    // template <typename T, typename... Args>
    // componentID_t addComponent(Args&&... args){
    //     return
    //     m_scene->instantiateEntityComponent<T>(m_id,std::forward<Args>(args)...);
    // }

    // // Add a new component of type T
    // template <typename T, typename... Args>
    // T* addComponent(Args &&...args) {
    //     static_assert(std::is_base_of<Component, T>::value, "T must inherit
    //     from Component");
    //     // // auto component =
    //     std::make_shared<T>(std::forward<Args>(args)...);
    //     // // component->m_Entity = this;
    //     // // m_components.emplace(typeid(T).hash_code(), component);
    //     // // return component;
    //     // auto component = m_c.insert(T(std::forward<Args>(args)...));
    //     // return *component;
    // }

    // // Get the first component of type T (if it exists)
    // template <typename T>
    // T *getComponent() {
    //     return m_scene->getComponentOfType<T>(getPtr());
    // }

    // // Get all components of type T
    // template <typename T>
    // std::vector<T *> getComponents() {
    //     return m_scene->getAllComponentsOfType<T>(getPtr());
    // }

    // Get all components
    std::vector<ComponentBase*> getComponents();

    // // Remove the first component of type T
    // template <typename T>
    // void removeComponent() {
    // }

    // // Remove a specific component by weak pointer
    // template <typename T>
    // void removeComponent(std::weak_ptr<T> component) {
    // }

    // // Remove all components of type T
    // template <typename T>
    // void removeAllComponents() {

    // }

    Entity() = default;
    Entity(Scene* scene, entityID_t uuid)
        : m_scene(scene), m_id(uuid), m_parent() {};
    Entity(Scene* scene, entityID_t uuid, entity_ptr parent)
        : m_scene(scene), m_id(uuid) {
        setParent(parent);
    };
    //    Entity(Entity&&) = default;
    //    Entity& operator=(Entity&&) = default;

    // Prevent copy
    //    Entity(const Entity&) = delete;
    //    Entity& operator=(const Entity&) = delete;

    ~Entity() {};

    void drawEditorGUI_Properties() override;
    void setParent(entity_ptr newParent);

    Scene* m_scene = nullptr;
    // TODO: Add reference to Transform and Rigidbody

   protected:
    std::string m_name = "entity";

    entityID_t m_id;
    entity_ptr m_parent;
    std::vector<entity_ptr> m_childs;
    std::vector<componentID_t> m_components;

    Transform* m_transform = nullptr;
    RigidBody* m_rigidBody = nullptr;

   private:
    void removeChild(entity_ptr child);
    void addChild(entity_ptr child);
};

}  // namespace v3d
