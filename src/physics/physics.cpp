#include "physics.h"

#include "physics/Vehicle.h"
#include "physics/collider.h"
#include "physics/rigidbody.h"

// ------------------------------- TEMP ----------------------------------
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono_vehicle/ChConfigVehicle.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/ChWorldFrame.h"
#include "chrono_vehicle/terrain/RigidTerrain.h"
#include "chrono_vehicle/utils/ChUtilsJSON.h"
#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledVehicle.h"
// ----------------------------------------------------------------------

namespace v3d {

Physics::Physics() {
    // test_chrono2();

    chrono::vehicle::ChWorldFrame::SetYUP();

    // // Rotation matrix from ISO -> custom frame
    // chrono::ChMatrix33<double> R(
    //     chrono::ChVector3d(1,  0,  0),
    //     chrono::ChVector3d(0.0000000, -0.4480736, -0.8939967),
    //     chrono::ChVector3d(0.0000000,  0.8939967, -0.4480736)
    // );

    // chrono::vehicle::ChWorldFrame::Set(R);

    chrono::vehicle::SetDataPath("resources/vehicle_model/");
    m_system.SetCollisionSystemType(chrono::ChCollisionSystem::Type::BULLET);
    // -9.8067
    m_system.SetGravitationalAcceleration(chrono::ChVector3d(0, -9.8067, 0));

    m_terrain =
        chrono_types::make_shared<chrono::vehicle::FlatTerrain>(-2, .8f);

    // double terrainLength = 50.0;
    // auto rigid_terrain =
    // chrono_types::make_shared<chrono::vehicle::RigidTerrain>(&m_system);
    // // auto patch_mat =
    // chrono_types::make_shared<chrono::ChContactMaterialSMC>();
    // // patch_mat->SetFriction(0.9f);
    // // patch_mat->SetRestitution(0.01f);
    // // patch_mat->SetYoungModulus(2e7f);
    // // patch_mat->SetPoissonRatio(0.3f);
    // // auto patch = rigid_terrain->AddPatch(patch_mat,
    // chrono::ChCoordsys<>(chrono::ChVector3d(0, -5, 0), chrono::QUNIT),
    // terrainLength, terrainLength);
    // // // patch->SetColor(ChColor(0.8f, 0.8f, 0.5f));
    // // //
    // patch->SetTexture(vehicle::GetDataFile("terrain/textures/tile4.jpg"),
    // 200, 5);
    // // rigid_terrain->Initialize();
    // m_terrain = rigid_terrain;
};

void Physics::addBody(RigidBody& body) {
    m_system.AddBody(body.m_body);
    // m_system.ShowHierarchy(std::cout);
}

void Physics::removeBody(RigidBody& body) {
    m_system.RemoveBody(body.m_body);
    // m_system.ShowHierarchy(std::cout);
}

// chrono::vehicle::WheeledVehicle* Physics::createVehicle(std::string
// vehicleModelPath){
//     // Create vehicle and driverInput
//     auto& vehicle = m_vehicles.emplace_back(&m_system, vehicleModelPath,
//     true, true); auto& driverInputs = m_driverInputs.emplace_back(); return
//     &vehicle;

//     // VehicleHandle handle;
//     // handle.vehicle = &vehicle;
//     // handle.driverInputs = &driverInputs;
//     // return handle;
// }

VehicleHandle Physics::createVehicle(std::string vehicleModelPath) {
    // Create vehicle
    auto& vehicle =
        m_vehicles.emplace_back(&m_system, vehicleModelPath, true, true);

    // Link vehicle to driver inputs
    // double steering
    // double throttle
    // double braking
    // double clutch
    m_vehicleInputs.push_back(
        {VehicleRaw_ptr(m_vehicles, m_vehicles.size() - 1),
         chrono::vehicle::DriverInputs({0, 1, 0, 0})});

    return VehicleHandle(m_vehicleInputs, m_vehicleInputs.size() -
                                              1);  // Provide pointer to Vehicle
}

void Physics::stepSimulation() {
    double simulationStepSize = m_step_size;
    double time = m_system.GetChTime();

    // Sync data between vehicle subsystems and terrain
    m_terrain->Synchronize(time);
    // for (auto vehicle : m_vehicles){
    //     // TODO: Store WheeledVehicle component instead and get DriverInputs
    //     from there vehicle.Synchronize(time, vehicle, *m_terrain);
    // }

    // for (int i=0; i < m_vehicles.size(); i++){
    //     auto vehicle = m_vehicles[i];
    //     auto driverInput = m_driverInputs[i];
    //     vehicle.Synchronize(time, driverInput, *m_terrain);
    // }

    for (int i = 0; i < m_vehicleInputs.size(); i++) {
        auto vehicle = m_vehicleInputs[i].vehicle;
        auto driverInput = m_vehicleInputs[i].driverInputs;
        vehicle->Synchronize(time, driverInput, *m_terrain);
    }

    // const auto steering = 0;
    // const auto throttle = 0;
    // const auto braking = 0;
    // const auto clutch = 1;

    // for (auto vehicle : m_vehicles){
    //     vehicle.Synchronize(time, chrono::vehicle::DriverInputs{steering,
    //     throttle, braking, clutch}, *m_terrain);;
    // }

    // // // // for (auto vehicleInput : m_vehicleInputs){
    // // // //     // vehicleInput.vehicle->Synchronize(time,
    // vehicleInput.driverInputs, *m_terrain);
    // // // //     vehicleInput.vehicle->Synchronize(time,
    // chrono::vehicle::DriverInputs{steering, throttle, braking, clutch},
    // *m_terrain);
    // // // // }

    // Advance simulation for one timestep for all modules
    m_terrain->Advance(simulationStepSize);
    for (auto vehicle : m_vehicles) {
        vehicle.Advance(simulationStepSize);
    }

    m_system.DoStepDynamics(simulationStepSize);
}

void Physics::printPosition() {
    auto bodies = m_system.GetBodies();
    for (auto body : bodies) {
        auto pos = body->GetPos();
        std::cout << pos << "\n";
    }
};
}  // namespace v3d
