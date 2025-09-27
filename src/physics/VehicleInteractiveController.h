#pragma once

#include "component.h"
#include "physics/DefinitionPhysics.hpp"

namespace v3d {
class VehicleInteractiveController : public ComponentBase {
   private:
    Vehicle *m_vehicle;

    float accelerate = 0;
    float back = 0;
    float brake = 0;
    float steerLeft = 0;
    float steerRight = 0;
    float clutch = 0;
    float throtle = 0;
    float steering = 0;

   public:
    VehicleInteractiveController() = default;
    ~VehicleInteractiveController() override = default;

    // // Remove Copy
    // VehicleInteractiveController(const VehicleInteractiveController&) =
    // delete; VehicleInteractiveController& operator=(
    //     const VehicleInteractiveController&) = delete;

    // // Enable move
    // VehicleInteractiveController(VehicleInteractiveController&&) = default;
    // VehicleInteractiveController& operator=(VehicleInteractiveController&&) =
    //     default;

    std::string getComponentName() override {
        return "VehicleInteractiveController";
    };

    void drawEditorGUI_Properties() override;

    void init() override;
    void start() override;
    void update(double deltaTime) override;
};
}  // namespace v3d
