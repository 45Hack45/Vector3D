#pragma once

#include "component.h"
#include "physics/DefinitionPhysics.hpp"

namespace v3d {
class RigidBody;
class Engine;
class Physics;
// class VehicleInteractiveController;

class Vehicle : public ComponentBase {
    friend Engine;
    friend Physics;
    // friend VehicleInteractiveController;
   private:
    std::string m_vehicleModelPath;
    RigidBody* m_rigidBody = nullptr;
    VehicleHandle m_vehicleHandle;

    chrono::ChVector3d m_initPos{0, .5, 0};
    chrono::ChQuaterniond m_initRot{1, 0, 0, 0};

    void loadVehicleModelJSON();

    // chrono::vehicle::WheeledVehicle* getVehicleRaw() { return
    // m_vehicle->vehicle; }; chrono::vehicle::DriverInputs*
    // getDriverInputsRaw() { return m_vehicle->driverInputs; };

    inline double getSteering() {
        return m_vehicleHandle->driverInputs.m_steering;
    }
    inline double getThrottle() {
        return m_vehicleHandle->driverInputs.m_throttle;
    }
    inline double getBraking() {
        return m_vehicleHandle->driverInputs.m_braking;
    }
    inline double getClutch() { return m_vehicleHandle->driverInputs.m_clutch; }

    inline void setSteering(double steering) {
        m_vehicleHandle->driverInputs.m_steering = steering;
    }
    inline void setThrottle(double throttle) {
        m_vehicleHandle->driverInputs.m_throttle = throttle;
    }
    inline void setBraking(double braking) {
        m_vehicleHandle->driverInputs.m_braking = braking;
    }
    inline void setClutch(double clutch) {
        m_vehicleHandle->driverInputs.m_clutch = clutch;
    }

    void resetDriverInputs() {
        m_vehicleHandle->driverInputs.m_steering = 0.0;
        m_vehicleHandle->driverInputs.m_throttle = 0.0;
        m_vehicleHandle->driverInputs.m_braking = 0.0;
        m_vehicleHandle->driverInputs.m_clutch = 0.0;
    }

   public:
    Vehicle() = default;
    ~Vehicle() override = default;

    // Remove Copy
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;

    // Enable move
    Vehicle(Vehicle&&) = default;
    Vehicle& operator=(Vehicle&&) = default;

    void init() override;
    void start() override;
    void update(double deltaTime) override;

    void onDrawGizmos(rendering::GizmosManager* gizmos) override;

    void setFilePath(const std::string& filepath) {
        m_vehicleModelPath = filepath;
    }

    void setInitialPosition(chrono::ChVector3d position) {
        m_initPos = position;
    }
    void setInitialRotation(chrono::ChQuaterniond rotation) {
        m_initRot = rotation;
    }
};

}  // namespace v3d