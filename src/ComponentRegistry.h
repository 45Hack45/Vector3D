#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "DefinitionCore.hpp"
#include "component.h"
#include "utils/keyed_stable_collection.hpp"

namespace v3d {

struct ComponentRegistrationInfo {
    std::string name;  // "Transform"
    std::type_index componentType = std::type_index(typeid(nullptr));
    /// @brief Factory returns a unique_ptr to a freshly constructed component
    /// (but not yet added)
    std::function<std::unique_ptr<ComponentBase>()> factory;
    /// @brief Factory returns a unique_ptr to an instance of a TypedVector,
    /// used to initialize the KeyedStableCollection internal container of a
    /// derived type at runetime without knowing the type at compile time.
    std::function<std::unique_ptr<ComponentMap::TypedVectorBase>()>
        componentCollectionFactory;
};

class ComponentRegistry {
   private:
    std::unordered_map<std::type_index, ComponentRegistrationInfo>
        m_componentRegistry;

   public:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;

    static ComponentRegistry& instance() {
        static ComponentRegistry instance;
        return instance;
    }

    /// @brief Register a component type (called once per type)
    bool registerComponent(ComponentRegistrationInfo& info);

    /// @brief Returns list of registered names (useful for UI)
    std::vector<std::string> getRegisteredNames() const;

    /// @brief Get component info by name
    /// @param name Component name
    /// @return const pointer if found or nullptr
    const ComponentRegistrationInfo* getInfo(const std::string& name) const;
    /// @brief Get component info by type
    /// @param typeIndex Component type
    /// @return const pointer if found or nullptr
    const ComponentRegistrationInfo* getInfo(std::type_index typeIndex) const;

    /// @brief Get component info of all registered components
    /// @return Vector of all registered components info
    std::vector<const ComponentRegistrationInfo*> getAllInfo() const;
};

template <typename, typename = void>
struct has_getName : std::false_type {};

template <typename T>
struct has_getName<T, std::void_t<decltype(T::getName())>> : std::true_type {};

template <typename C>
struct ComponentRegistrar {
    std::string name = "";
    ComponentRegistrar() {
        static_assert(
            has_getName<C>::value,
            "Derived class must define: static std::string getName()");

        name = C::getName();
        ComponentRegistrationInfo info{
            name, std::type_index(typeid(C)),
            []() -> std::unique_ptr<ComponentBase> {
                return std::make_unique<C>();
            },
            []() -> std::unique_ptr<ComponentMap::TypedVectorBase> {
                return std::make_unique<ComponentMap::TypedVector<C>>();
            }};

        // Test factories
        info.factory();
        info.componentCollectionFactory();
        ComponentRegistry::instance().registerComponent(info);
    }
    ~ComponentRegistrar() = default;
};

}  // namespace v3d

#define REGISTER_COMPONENT(Type) \
    static v3d::ComponentRegistrar<Type> registrar_##Type;
