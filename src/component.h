#pragma once

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <string>
#include <typeinfo>

#include "DefinitionCore.hpp"
#include "rendering/rendering_def.h"
#include "utils/keyed_stable_collection.hpp"

namespace v3d {
class Entity;
class Scene;
class ComponentBase;

using ComponentMap = utils::KeyedStableCollection<componentID_t, ComponentBase>;

class ComponentBase : public rendering::IGizmosRenderable, public IEditorGUISelectable {
    friend class Entity;
    friend class Scene;
    friend class boost::serialization::access;

   public:
    ComponentBase() = default;
    virtual ~ComponentBase();

    virtual void init() {};
    virtual void start() = 0;
    virtual void update(double deltaTime) = 0;

    virtual std::string getComponentName() = 0;

    static auto dependencies() { return std::tuple<>(); }

    entityID_t getEntity() { return m_entity; }
    entity_ptr getEntityPtr();

    std::string getEntityName();

    virtual void drawEditorGUI_Properties() {
        // log_error("ERROR::COMPONENT::BASE_CLASS_VIRTUAL_METHOD_CALLED:
        // drawEditorGUI_Properties\n");
        ImGui::Text("No editor GUI available for %s", getComponentName().c_str());
    };

   private:
    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        std::string componentId = boost::uuids::to_string(m_id);
        std::string entityId = boost::uuids::to_string(m_entity);

        ar& BOOST_SERIALIZATION_NVP(componentId);
        ar& BOOST_SERIALIZATION_NVP(entityId);
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        std::string componentId;
        std::string entityId;

        ar& BOOST_SERIALIZATION_NVP(componentId);
        ar& BOOST_SERIALIZATION_NVP(entityId);

        m_id = boost::uuids::string_generator()(componentId);
        m_entity = boost::uuids::string_generator()(entityId);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()

   protected:
    componentID_t m_id;
    entityID_t m_entity;
    Scene* m_scene = nullptr;

    void setEntity(entityID_t entity) { m_entity = entity; }

    // Initialize base component, call once after component creation
    void _init();
};

class DataComponent : public ComponentBase {
   public:
    DataComponent() = default;
    void update(double deltaTime) {};
    virtual void start() {};
};

class TestComponent : public ComponentBase {
   public:
    TestComponent() = default;
    TestComponent(int value) : testVariable(value) {};
    bool test_method() { return true; }

    void start() override {};
    void update(double deltaTime) override;

    std::string getComponentName() override { return "TestComponent"; };
    static std::string getName() { return "TestComponent"; };

   private:
    int testVariable;
};

class AbsoluteASCIIComponent : public ComponentBase {
   public:
    AbsoluteASCIIComponent() = default;
    AbsoluteASCIIComponent(int value) : testVariable(value) {};
    bool test_method() { return true; }

    void start() override {};
    void update(double deltaTime) override;

    std::string getComponentName() override { return "AbsoluteASCIIComponent"; };
    static std::string getName() { return "AbsoluteASCIIComponent"; };

   private:
    int testVariable;
};

class CinemaASCIIComponent : public ComponentBase {
   public:
    CinemaASCIIComponent() = default;
    CinemaASCIIComponent(int value) : testVariable(value) {};
    bool test_method() { return true; }

    static auto dependencies() { return std::tuple<AbsoluteASCIIComponent>{}; }
    void start() override {};
    void update(double deltaTime) override;

    std::string getComponentName() override { return "CinemaASCIIComponent"; };
    static std::string getName() { return "CinemaASCIIComponent"; };

   private:
    int testVariable;
};

class TestDataComponent : public DataComponent {
   public:
    TestDataComponent() = default;
    void start() override {};
    void update(double deltaTime) override {};

    std::string getComponentName() override { return "TestDataComponent"; };

    int testDataComponentField;
};

}  // namespace v3d
