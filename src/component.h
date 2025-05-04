#pragma once

#include "utils/keyed_stable_collection.hpp"
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <string>
#include <typeinfo>

namespace v3d {
class Entity;
class Scene;
class ComponentBase;

typedef boost::uuids::uuid componentID_t;
using ComponentMap = utils::KeyedStableCollection<componentID_t, ComponentBase>;

class ComponentBase {
    friend class Entity;
    friend class Scene;

   public:
    ComponentBase() = default;
    virtual ~ComponentBase() = default;

    virtual void start() = 0;
    virtual void update(double deltaTime) = 0;

    virtual std::string getComponentName() {
        return typeid(this).name();
    }

    static auto dependencies() {
        return std::tuple<>();
    }

   private:
   protected:
    componentID_t m_id;
    Entity *m_entity;

    void setEntity(Entity *entity) {
        m_entity = entity;
    }
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

   private:
    int testVariable;
};

class CinemaASCIIComponent : public ComponentBase {
   public:
    CinemaASCIIComponent() = default;
    CinemaASCIIComponent(int value) : testVariable(value) {};
    bool test_method() { return true; }

    static auto dependencies() {
        return std::tuple<AbsoluteASCIIComponent>{};
    }
    void start() override {};
    void update(double deltaTime) override;

   private:
    int testVariable;
};

class TestDataComponent : public DataComponent {
   public:
    TestDataComponent() = default;
    void start() override {};
    void update(double deltaTime) override {};
    int testDataComponentField;
};

}  // namespace v3d
