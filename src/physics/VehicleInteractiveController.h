#pragma once

#include "component.h"
#include "physics/DefinitionPhysics.hpp"

namespace v3d {
class VehicleInteractiveController : public ComponentBase {
    friend class boost::serialization::access;
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
    static std::string getName() { return "VehicleInteractiveController"; };

    void drawEditorGUI_Properties() override;

    void init() override;
    void start() override;
    void update(double deltaTime) override;

   private:
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar& boost::serialization::make_nvp(
            "ComponentBase",
            boost::serialization::base_object<ComponentBase>(*this));
        // All float control inputs are transient; they reset each update() from
        // the InputManager and are not persisted.
    }
};
}  // namespace v3d
