#pragma once

#include <boost/uuid/uuid.hpp>
#include <memory>
#include <string>
#include <typeinfo>

namespace v3d {
class Entity;
class Scene;
typedef boost::uuids::uuid componentID_t;
class Component {
    friend class Entity;
    friend class Scene;

   public:
    Component() = default;
    virtual ~Component() = default;

    virtual void start() = 0;
    virtual void update(double deltaTime) = 0;

    virtual std::string getComponentName() {
        return typeid(this).name();
    }

   private:
   protected:
    componentID_t m_id;
    Entity *m_entity;
};

class DataComponent : public Component {
   public:
    DataComponent() = default;
    void update(double deltaTime) {};
    virtual void start() {};
};

class TestComponent : public Component {
    public:
     TestComponent() = default;
     TestComponent(int value) : testVariable(value) {};
     bool test_method() { return true; }
 
     void start() override {};
     void update(double deltaTime) override;
 
    private:
     int testVariable;
 };

class AbsoluteASCIIComponent : public Component {
   public:
    AbsoluteASCIIComponent() = default;
    AbsoluteASCIIComponent(int value) : testVariable(value) {};
    bool test_method() { return true; }

    void start() override {};
    void update(double deltaTime) override;

   private:
    int testVariable;
};

class CinemaASCIIComponent : public Component {
    public:
     CinemaASCIIComponent() = default;
     CinemaASCIIComponent(int value) : testVariable(value) {};
     bool test_method() { return true; }
 
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
