#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "DefinitionCore.hpp"
#include "component.h"
#include "entity.h"
#include "object_ptr.hpp"
#include "utils/utils.hpp"

namespace v3d {
class Engine;
class Physics;

const uint8_t MAX_ENTITY_NESTED_DEPTH = 255;

class Scene {
    friend class Engine;

    struct Private {
        explicit Private() = default;
    };

   public:
    /**
     * @brief Private constructor
     * @param
     */
    Scene(Private) {};
    ~Scene() {};

    /**
     * @brief Scene factory
     * @return
     */
    static std::shared_ptr<Scene> create(Engine* engine, Physics* physics) {
        auto scene = std::make_shared<Scene>(Private());
        scene->m_engine = engine;
        scene->m_phSystem = physics;
        scene->init();
        return scene;
    }

    entity_ptr instantiateEntity(std::string name, entity_ptr parent);
    entity_ptr instantiateEntity(std::string name) {
        return this->instantiateEntity(name, m_root);
    }

    template <typename T, typename... Args>
    componentID_t instantiateEntityComponent(entity_ptr entity,
                                             Args&&... args) {
        static_assert(std::is_base_of_v<ComponentBase, T>,
                      "T must inherit from ComponentBase");

        // Instantiate all unmet dependencies first
        utils::forEachInTuple(T::dependencies(), [this, entity](auto dummy) {
            // Calc dependancy component specific type
            using Dep = std::decay_t<decltype(dummy)>;

            // Check if the entity has the component
            if (this->hasComponent<Dep>(entity)) return;

            // Create component and its recursive dependancies
            this->instantiateEntityComponent<Dep>(
                entity);  // Each gets its own UUID
        });

        // Create Component
        componentID_t uuid = boost::uuids::random_generator()();
        m_components.insert<T>(uuid, std::forward<Args>(args)...);
        // Assign entity to component and vice versa
        auto component = m_components.get(uuid);
        component->m_scene = this;
        component->m_entity = entity.index();
        entity->m_components.push_back(uuid);

        // Initialize component base
        component->_init();

        // Initialize component specialization
        component->init();
        component->start();

        return uuid;
    }

    template <typename T, typename... Args>
    T* createEntityComponentOfType(entity_ptr entity, Args&&... args) {
        auto component_id =
            instantiateEntityComponent<T>(entity, std::forward<Args>(args)...);
        return getComponent<T>(component_id);
    }

    entity_ptr getEntity(entityID_t entityID) {
        if (m_entities.contains(entityID)) {
            return entity_ptr(m_entities, entityID);
        } else {
            return entity_ptr();
        }
    }

    // Get instance of a specific component
    template <typename T>
    T* getComponent(componentID_t componentID) {
        return m_components.getAs<T>(componentID);
    }

    // Get first component of an entity of type T if found
    template <typename T>
    T* getComponentOfType(entity_ptr entity) {
        T* component = nullptr;
        for (size_t i = 0; i < entity->m_components.size(); i++) {
            auto cp = entity->m_components[i];
            component = m_components.getAs<T>(cp);
            if (component != nullptr) {
                // Component found
                break;
            }
        }
        return component;
    }

    // Get first component of an entity of type T if found
    template <typename T>
    T* getComponentOfType(entityID_t entityID) {
        entity_ptr entity = getEntity(entityID);
        return getComponentOfType<T>(entity);
    }

    // Get any component of type T if found
    template <typename T>
    T* getComponentOfType() {
        return m_components.getFirstOfType<T>();
    }

    template <typename T>
    std::vector<T*> getEntityComponents(entity_ptr entity) {
        std::vector<T*> componentList;
        for (auto const component : entity->m_components) {
            auto component_ptr = m_components.getAs<T>(component);
            if (component_ptr != nullptr) componentList.pushBack();
        }
        return componentList;
    }

    template <typename T>
    bool hasComponent(entity_ptr entity) {
        bool found = false;
        for (size_t i = 0; i < entity->m_components.size(); i++) {
            auto component = entity->m_components[i];
            if (m_components.getAs<T>(component)) {
                found = true;
                break;
            }
        }
        return found;
    }

    void deleteEntity(Entity* entity) {
        // TODO: Deleyed destroy, mark object as to be deleted and delete after
        // the frame update
    }

    void update(double delta) {
        m_components.for_each(
            [delta](ComponentBase& component) { component.update(delta); });
    }

    void print_entities() {
        for (auto [id, entity] : m_entities) {
            if (entity.m_parent) {
                std::cout << "Entity: " << entity.m_name
                          << " Parent: " << entity.m_parent->m_name << "\n";
            } else {
                std::cout << "Entity: " << entity.m_name << "\n";
            }
        }
    }

    Engine* getEngine() { return m_engine; }
    Physics* getPhysics() { return m_phSystem; }

   private:
    Engine* m_engine;
    Physics* m_phSystem;
    entity_ptr m_root;
    EntityMap m_entities;
    ComponentMap m_components;

    void init() {
        // Clear existing entities
        if (m_entities.size()) {
            m_entities.clear();
        }

        m_root = createEntity();
        m_root->m_name = "root";
    };

    entity_ptr createEntity() {
        entityID_t uuid = boost::uuids::random_generator()();
        auto [entity_it, inserted] =
            m_entities.emplace(uuid, Entity(this, uuid));
        return entity_ptr(m_entities, uuid);
    };
    entity_ptr createEntity(entity_ptr parent) {
        entity_ptr entity = createEntity();
        entity->m_parent = parent;
        parent->m_childs.push_back(entity);
        return entity;
    };
};

}  // namespace v3d
