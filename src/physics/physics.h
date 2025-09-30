#pragma once
#include "chrono/core/ChRealtimeStep.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono_vehicle/terrain/FlatTerrain.h"
#include "physics/DefinitionPhysics.hpp"
#include "physics/utils.hpp"

namespace v3d {
class Engine;
class RigidBody;
class ColliderBase;

class Physics {
    friend class Engine;

   public:
    Physics();
    ~Physics() {};

    void addBody(
        RigidBody &body);  // TOOD: Refactor addBody to createBody, instead of
                           // registering the body create it directly from
                           // Physics because it owns the resource
    void removeBody(RigidBody &body);

    VehicleHandle createVehicle(std::string vehicleModelPath);

    std::shared_ptr<chrono::ChContactMaterialSMC>
    getDefaultCollisionMaterial() {
        return m_defaultCollMat;
    }

   private:
    chrono::ChSystemSMC m_system;
    std::shared_ptr<chrono::ChContactMaterialSMC> m_defaultCollMat =
        chrono_types::make_shared<chrono::ChContactMaterialSMC>();

    // Vehicle Simulation
    std::shared_ptr<chrono::vehicle::ChTerrain> m_terrain;
    std::vector<chrono::vehicle::WheeledVehicle> m_vehicles;
    std::vector<VehicleInputs> m_vehicleInputs;
    // std::vector<chrono::vehicle::DriverInputs> m_driverInputs;

    void stepSimulation();

    // Enforce soft-realtime
    void spin(double step_size) { realtime_timer.Spin(step_size); }

    // Simulation step sizes
    double m_step_size = 4e-4;
    chrono::ChRealtimeStepTimer realtime_timer;

    void printPosition();
    void renderDebbugGUI();
};

}  // namespace v3d
