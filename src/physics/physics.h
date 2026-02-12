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
class ConstrainLink;

class Physics {
    friend class Engine;

   public:
    Physics();
    ~Physics() {};

    void addBody(
        RigidBody& body);  // TOOD: Refactor addBody to createBody, instead of
                           // registering the body create it directly from
                           // Physics because it owns the resource

    /// @brief Add a rigidbody to the physics system
    /// @param body RigidBody
    void addBody(std::shared_ptr<chrono::ChBody> body);
    void removeBody(RigidBody& body);
    /// @brief Remove a rigidbody to the physics system
    /// @param body RigidBody
    void removeBody(std::shared_ptr<chrono::ChBody> body);

    void addLink(ConstrainLink& link);
    void removeLink(ConstrainLink& link);

    VehicleHandle createVehicle(std::string vehicleModelPath);

    std::shared_ptr<chrono::ChContactMaterialSMC>
    getDefaultCollisionMaterial() {
        return m_defaultCollMat;
    }

    /// @brief Write the hierarchy of contained bodies to standard output
    void showHierarchy();

    inline double getStepSize() { return m_step_size; }
    inline void setStepSize(double stepSize) { m_step_size = stepSize; }

    inline double getStepPerFrame() { return m_stepPerFrame; }
    inline void setStepPerFrame(int stepPerFrame) { m_stepPerFrame = stepPerFrame; }

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

    // Max simulation steps per frame
    int m_stepPerFrame = 20;

    void printPosition();
    void renderDebbugGUI();
};

}  // namespace v3d
