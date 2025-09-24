#pragma once

#include "glm/glm.hpp"
#include "physics/DefinitionPhysics.hpp"

namespace v3d {
namespace physics {
// Convert glm -> chrono
inline chrono::ChVector3d toChrono(const glm::vec3 &v) {
    return chrono::ChVector3d(v.x, v.y, v.z);
}

// Convert chrono -> glm
inline glm::vec3 toV3d(const chrono::ChVector3d &v) {
    return glm::vec3(v.x(), v.y(), v.z());  // ChVector3d stores as double
}
}  // namespace physics

}  // namespace v3d
