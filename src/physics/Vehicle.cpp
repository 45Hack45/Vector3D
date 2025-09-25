
#include "physics/Vehicle.h"

#include "Vehicle.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/utils/ChUtilsJSON.h"
#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledTrailer.h"
#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledVehicle.h"
#include "physics/physics.h"
#include "physics/rigidbody.h"
#include "physics/utils.hpp"
#include "rendering/graphics_backend.h"
#include "scene.h"

namespace v3d {

void Vehicle::init() {
    // Set the rigidbody to this instance
    auto rigidBody = m_scene->getComponentOfType<RigidBody>(m_entity);
    assert(rigidBody != nullptr &&
           "Failed to initialize Transform. Missing required component "
           "Transform from the entity");
    m_rigidBody = rigidBody;
}

void Vehicle::start() {
    if (!m_vehicleModelPath.empty()) {
        // Load
        loadVehicleModelJSON();
    }
}

void Vehicle::update(double deltaTime) {
    // // if (m_vehicle) std::cout << "Vehicle pos " <<
    // m_vehicle->GetChassis()->GetPos() << "\n";
    // // m_vehicle->EnableRealtime(true);
    // if (true){
    //     std::cout << "Vehicle pos " <<
    //     m_vehicleHandle->vehicle->GetChassis()->GetPos() << "\n";
    //     // // std::cout << "Steearing angle: " <<
    //     m_vehicleHandle->vehicle->GetSteeringAngle() << "\n";

    //     // auto engine = m_vehicleHandle->vehicle->GetEngine();
    //     // if (engine) {
    //     //     std::cout << "Vehicle engine: " <<
    //     engine->GetOutputMotorshaftTorque() << "\n";
    //     // } else {
    //     //     std::cout << "engine not available\n";
    //     // }
    //     // auto test = engine.get();
    //     // auto test2 = engine->GetMotorSpeed();
    //     // std::cout << "Motor speed: " << test << "\n";
    // }

    // auto vehicle = m_vehicleHandle->vehicle;
    // // std::cout << vehicle->GetChassisBody()->GetRotAxis() << "\n";
}

void Vehicle::onDrawGizmos(rendering::GizmosManager* gizmos) {
    auto vehicle = m_vehicleHandle->vehicle;

    // gizmos->draw_cube(physics::toV3d(vehicle->GetPos()), .25,
    //                   glm::vec4(1, 0, 0, 1));

    // // Draw chasis
    // gizmos->draw_cube(physics::toV3d(vehicle->GetChassisBody()->GetPos()),
    // .51,
    //                   glm::vec4(1, 0, 0, 1));

    // Draw wheels
    for (auto axle : vehicle->GetAxles()) {
        for (auto wheel : axle->GetWheels()) {
            gizmos->draw_sphere(physics::toV3d(wheel->GetPos()), .2);
        }
    }
}

void Vehicle::loadVehicleModelJSON() {
    assert(!m_vehicleModelPath.empty() &&
           "Tried to load Vehicle JSON but vehicleModelPath not defined");
    // assert(!m_vehicle.vehicle && "Tried to load Vehicle JSON has already been
    // loaded");  // Can only load once
    assert(
        m_rigidBody &&
        "Tried to load Vehicle JSON but rigidbody not defined");  // Can only
                                                                  // load once

    // Load model

    Physics* physics = m_scene->getPhysics();

    // m_vehicle =
    // chrono_types::make_unique<chrono::vehicle::WheeledVehicle>(&physics->m_system,
    // m_vehicleModelPath, true, true);
    m_vehicleHandle = physics->createVehicle(m_vehicleModelPath);

    auto vehicle = m_vehicleHandle->vehicle;
    vehicle->Initialize(chrono::ChCoordsys<>(m_initPos, m_initRot));
    vehicle->GetChassisBody()->SetFixed(false);

    // Register vehicle

    // Set rigidbody internal body to vehicle chasis body
    m_rigidBody->hardResetBody(
        (chrono::ChBody*)vehicle->GetChassisBody().get());
}

}  // namespace v3d
