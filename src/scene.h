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

#include "utils/keyed_stable_collection.hpp"
#include "component.h"
#include "Entity.h"
#include "object_ptr.hpp"

class Entity;

namespace v3d {
const uint8_t MAX_ENTITY_NESTED_DEPTH = 255;

class Scene : public std::enable_shared_from_this<Scene> {
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
    static std::shared_ptr<Scene> create() {
        auto scene = std::make_shared<Scene>(Private());
        scene->init();
        return scene;
    }

    std::shared_ptr<Scene> getptr() {
        return shared_from_this();
    }

    entity_ptr instantiateEntity(std::string name, entity_ptr parent) {
        entity_ptr entity = createEntity(parent);
        entity->m_name = name;
        return entity;
    }

    entity_ptr instantiateEntity(std::string name) {
        return this->instantiateEntity(name, m_root);
    }

    template <typename T, typename... Args>
    componentID_t instantiateEntityComponent(const entityID_t entity_id, Args&&... args){
        componentID_t uuid = boost::uuids::random_generator()();
        m_components.insert<T>(uuid, std::forward<Args>(args)...);
        auto component = m_components.get(uuid);
        component->m_entity = &m_entities[entity_id];
        return uuid;
    }
    
    template <typename T>
    T* getComponent(componentID_t component_id){
        return m_components.getAs<T>(component_id);
    }

    void deleteEntity(Entity* entity) {
        //TODO: Deleyed destroy, mark object as to be deleted and delete after the frame update
    }

    void update(double delta) {
        m_components.for_each([delta](Component& component) { component.update(delta); });
    }

    void print_entities() {
        for (auto [id, entity] : m_entities) {
            if (entity.m_parent) {
                std::cout << "Entity: " << entity.m_name << " Parent: " << entity.m_parent->m_name << "\n";
            } else {
                std::cout << "Entity: " << entity.m_name << "\n";
            }
        }
    }

   private:
    entity_ptr m_root;
    EntityMap m_entities;
    utils::KeyedStableCollection<componentID_t, Component> m_components;

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
        auto [entity_it, inserted] = m_entities.emplace(uuid, Entity(this, uuid));
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
