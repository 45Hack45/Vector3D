#pragma once
#include <boost/container/flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid/uuid.hpp>

#include "imgui.h"
#include "object_ptr.hpp"

namespace v3d {

// Entity
class Entity;

typedef boost::uuids::uuid entityID_t;
typedef boost::uuids::uuid componentID_t;

// typedef utils::vector_ptr<Entity> entity_ptr;
struct EntityUuidHash {
    std::size_t operator()(const entityID_t& uuid) const noexcept {
        return boost::hash_range(uuid.begin(), uuid.end());
    }
};
using EntityMap = boost::unordered_flat_map<entityID_t, Entity, EntityUuidHash>;
using entity_ptr = object_ptr<EntityMap, Entity, entityID_t>;

// Component
class ComponentBase;

// Editor
class IEditorGUISelectable {
   public:
    virtual void drawEditorGUI_Properties() = 0;
};

}  // namespace v3d
