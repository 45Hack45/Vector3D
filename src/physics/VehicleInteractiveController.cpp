
#include "physics/VehicleInteractiveController.h"

#include "engine.h"
#include "input/InputKeys.hpp"
#include "physics/Vehicle.h"
#include "scene.h"

namespace v3d {
void VehicleInteractiveController::init() {
    // Set the vehicle to this instance
    auto vehicle = m_scene->getComponentOfType<Vehicle>(m_entity);
    assert(vehicle != nullptr &&
           "Failed to initialize Transform. Missing required component "
           "Transform from the entity");
    m_vehicle = vehicle;
}

void VehicleInteractiveController::start() {}

void VehicleInteractiveController::update(double deltaTime) {
    auto inputManager = m_scene->getEngine()->getInputManager();

    float accelerate = inputManager->getAction(input::action::IAct_Accelerate);
    float back = inputManager->getAction(input::action::IAct_Back);
    float brake = inputManager->getAction(input::action::IAct_Break);
    float steerLeft = inputManager->getAction(input::action::IAct_SteerLeft);
    float steerRight = inputManager->getAction(input::action::IAct_SteerRight);
    float clutch = inputManager->getAction(input::action::IAct_Clutch);

    float throtle = accelerate - back;
    float steering = steerLeft - steerRight;

    m_vehicle->setThrottle(throtle);
    m_vehicle->setBraking(brake);
    m_vehicle->setSteering(steering);
    m_vehicle->setClutch(clutch);
}
}  // namespace v3d
