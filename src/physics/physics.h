#pragma once
#include "chrono/core/ChRealtimeStep.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono_vehicle/terrain/FlatTerrain.h"
#include "physics/DefinitionPhysics.hpp"

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

    std::shared_ptr<chrono::ChContactMaterialNSC>
    getDefaultCollisionMaterial() {
        return m_defaultCollMat;
    }

   private:
    chrono::ChSystemNSC m_system;
    std::shared_ptr<chrono::ChContactMaterialNSC> m_defaultCollMat =
        chrono_types::make_shared<chrono::ChContactMaterialNSC>();

    // Vehicle Simulation
    std::shared_ptr<chrono::vehicle::ChTerrain> m_terrain;
    std::vector<chrono::vehicle::WheeledVehicle> m_vehicles;
    std::vector<VehicleInputs> m_vehicleInputs;
    // std::vector<chrono::vehicle::DriverInputs> m_driverInputs;

    void stepSimulation();

    // Enforce soft-realtime
    void spin(double step_size) { realtime_timer.Spin(m_step_size); }

    // Simulation step sizes
    const double m_step_size = 1e-3;
    chrono::ChRealtimeStepTimer realtime_timer;

    void printPosition();
};

}  // namespace v3d
