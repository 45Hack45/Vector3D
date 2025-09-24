#pragma once

#include "component.h"
#include "physics/DefinitionPhysics.hpp"

namespace v3d {
class Vehicle;
class VehicleInteractiveController : public ComponentBase {
   private:
    chrono::vehicle::ChInteractiveDriver m_driver;
    Vehicle* m_vehicle;

   public:
    VehicleInteractiveController() = default;
    ~VehicleInteractiveController() override = default;

    // Remove Copy
    VehicleInteractiveController(const VehicleInteractiveController&) = delete;
    VehicleInteractiveController& operator=(const VehicleInteractiveController&) = delete;

    // Enable move
    VehicleInteractiveController(VehicleInteractiveController&&) = default;
    VehicleInteractiveController& operator=(VehicleInteractiveController&&) = default;

    void init() override;
    void start() override;
    void update(double deltaTime) override;
};
}  // namespace v3d
