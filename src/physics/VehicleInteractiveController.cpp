
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

    if (!inputManager->isMuted()) {
        accelerate = inputManager->getAction(input::action::IAct_Accelerate);
        back = inputManager->getAction(input::action::IAct_Back);
        brake = inputManager->getAction(input::action::IAct_Break);
        steerLeft = inputManager->getAction(input::action::IAct_SteerLeft);
        steerRight = inputManager->getAction(input::action::IAct_SteerRight);
        clutch = inputManager->getAction(input::action::IAct_Clutch);
    }

    throtle = accelerate - back;
    steering = steerLeft - steerRight;

    m_vehicle->setThrottle(throtle);
    m_vehicle->setBraking(brake);
    m_vehicle->setSteering(steering);
    m_vehicle->setClutch(clutch);
}

void VehicleInteractiveController::drawEditorGUI_Properties() {
    ImGui::Text("Raw Inputs");
    ImGui::SliderFloat("Accelerate", &accelerate, 0, 1);
    ImGui::SliderFloat("Back", &back, 0, 1);
    ImGui::SliderFloat("Brake", &brake, 0, 1);
    ImGui::SliderFloat("SteerLeft", &steerLeft, 0, 1);
    ImGui::SliderFloat("SteerRight", &steerRight, 0, 1);
    ImGui::SliderFloat("Clutch", &clutch, 0, 1);

    ImGui::Spacing();

    ImGui::Text("Compound Input");
    ImGui::SliderFloat("Throtle", &throtle, -1, 1, "%.3f",
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::SliderFloat("Steering", &steering, -1, 1, "%.3f",
                       ImGuiInputTextFlags_ReadOnly);
}

}  // namespace v3d
