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
    friend class boost::serialization::access;
    // friend VehicleInteractiveController;
   private:
    std::string m_vehicleModelPath;
    bool m_vehicleModelPathDirty = false;
    bool m_isLoaded = false;

    RigidBody* m_rigidBody = nullptr;
    VehicleHandle m_vehicleHandle;
    bool m_parkingBrake = false;  // Apply parking brake

    chrono::ChVector3d m_initPos{0, .5, 0};
    chrono::ChQuaterniond m_initRot{1, 0, 0, 0};
    double m_initSpeed = 0;

    void loadVehicleModelJSON();

    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        ar& BOOST_SERIALIZATION_NVP(m_vehicleModelPath);
        ar& BOOST_SERIALIZATION_NVP(m_parkingBrake);

        // Initial physics state
        double speed = m_initSpeed;
        chrono::ChVector3d position = m_initPos;
        chrono::ChQuaterniond rotation = m_initRot;
        if (m_isLoaded) {
            auto vehicle = m_vehicleHandle.get().vehicle;
            speed = vehicle->GetSpeed();
            position = vehicle->GetPos();
            rotation = vehicle->GetRot();
        }

        ar& boost::serialization::make_nvp("speed", speed);

        ar& boost::serialization::make_nvp("positionX", position.x());
        ar& boost::serialization::make_nvp("positionY", position.y());
        ar& boost::serialization::make_nvp("positionZ", position.z());

        ar& boost::serialization::make_nvp("rotationE0", rotation.e0());
        ar& boost::serialization::make_nvp("rotationE1", rotation.e1());
        ar& boost::serialization::make_nvp("rotationE2", rotation.e2());
        ar& boost::serialization::make_nvp("rotationE3", rotation.e3());

        // m_vehicleHandle, m_rigidBody, m_isLoaded are runtime state rebuilt
        // by start() using the restored m_vehicleModelPath and init pose.
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        ar& BOOST_SERIALIZATION_NVP(m_vehicleModelPath);
        ar& BOOST_SERIALIZATION_NVP(m_parkingBrake);

        // Load initial physics state

        double initSpeed = 0;
        ar& boost::serialization::make_nvp("speed", initSpeed);
        m_initSpeed = initSpeed;

        double initPosX, initPosY, initPosZ;
        ar& boost::serialization::make_nvp("positionX", initPosX);
        ar& boost::serialization::make_nvp("positionY", initPosY);
        ar& boost::serialization::make_nvp("positionZ", initPosZ);
        m_initPos = chrono::ChVector3d(initPosX, initPosY, initPosZ);

        double initRotE0, initRotE1, initRotE2, initRotE3;
        ar& boost::serialization::make_nvp("rotationE0", initRotE0);
        ar& boost::serialization::make_nvp("rotationE1", initRotE1);
        ar& boost::serialization::make_nvp("rotationE2", initRotE2);
        ar& boost::serialization::make_nvp("rotationE3", initRotE3);
        m_initRot = chrono::ChQuaterniond(initRotE0, initRotE1, initRotE2, initRotE3);

        m_vehicleModelPathDirty = true;
        m_isLoaded = false;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()

    // chrono::vehicle::WheeledVehicle* getVehicleRaw() { return
    // m_vehicle->vehicle; }; chrono::vehicle::DriverInputs*
    // getDriverInputsRaw() { return m_vehicle->driverInputs; };

   public:
    Vehicle() = default;
    // TODO: Restore the Rigidbody chBody when the vehicle instance is destroyed
    ~Vehicle() override = default;

    // Remove Copy
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;

    // Enable move
    Vehicle(Vehicle&&) = default;
    Vehicle& operator=(Vehicle&&) = default;

    std::string getComponentName() override { return "Vehicle"; };
    static std::string getName() { return "Vehicle"; };

    void init() override;
    void start() override;
    void update(double deltaTime) override;

    void onDrawGizmos(rendering::GizmosManager* gizmos) override;

    void drawEditorGUI_Properties() override;

    void setFilePath(const std::string& filepath) {
        m_vehicleModelPathDirty = true;
        m_vehicleModelPath = filepath;
    }

    void setInitialPosition(chrono::ChVector3d position) {
        m_initPos = position;
    }
    void setInitialRotation(chrono::ChQuaterniond rotation) {
        m_initRot = rotation;
    }

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

    inline void applyParking(bool parking) { m_parkingBrake = parking; }
    inline bool isParked() { return m_parkingBrake; }
};

}  // namespace v3d