#pragma once

#include <boost/poly_collection/base_collection.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "object_ptr.hpp"
#include "scene.h"
#include "utils/definitions.hpp"
// #include "utils/vector_ptr.hpp"

namespace v3d {

// typedef utils::vector_ptr<Entity> entity_ptr;
struct EntityUuidHash {
    std::size_t operator()(const entityID_t &uuid) const noexcept {
        return boost::hash_range(uuid.begin(), uuid.end());
    }
};
using EntityMap = boost::unordered_flat_map<entityID_t, Entity, EntityUuidHash>;
using entity_ptr = object_ptr<EntityMap, Entity>;

class Entity {
    friend class Scene;

   public:
    const std::vector<entity_ptr> &getChilds() const {
        return m_childs;
    }

    // template <typename T, typename... Args>
    // componentID_t addComponent(Args&&... args){
    //     return m_scene->instantiateEntityComponent<T>(m_id,std::forward<Args>(args)...);
    // }

    // // Add a new component of type T
    // template <typename T, typename... Args>
    // T* addComponent(Args &&...args) {
    //     static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
    //     // // auto component = std::make_shared<T>(std::forward<Args>(args)...);
    //     // // component->m_Entity = this;
    //     // // m_components.emplace(typeid(T).hash_code(), component);
    //     // // return component;
    //     // auto component = m_c.insert(T(std::forward<Args>(args)...));
    //     // return *component;
    // }

    // Get the first component of type T (if it exists)
    template <typename T>
    std::weak_ptr<T> getComponent() {
        return {};
    }

    // // Get all components of type T
    // template <typename T>
    // std::vector<std::weak_ptr<T>> getComponents() {
    //     return result;
    // }

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

    std::string m_name = "entity";

    Entity() = default;
    Entity(Scene *scene, entityID_t uuid) : m_scene(scene), m_id(uuid), m_parent() {};
    Entity(Scene *scene, entityID_t uuid, entity_ptr parent) : m_scene(scene), m_id(uuid), m_parent(parent) {};
    //    Entity(Entity&&) = default;
    //    Entity& operator=(Entity&&) = default;

    // Prevent copy
    //    Entity(const Entity&) = delete;
    //    Entity& operator=(const Entity&) = delete;

    ~Entity() {};

    Scene *m_scene;
    // TODO: Add reference to Transform and Rigidbody

   protected:
    entityID_t m_id;
    entity_ptr m_parent;
    std::vector<entity_ptr> m_childs;
    std::vector<componentID_t> m_components;

   private:
};

}  // namespace v3d
