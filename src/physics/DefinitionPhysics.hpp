#pragma once

#include "object_ptr.hpp"
#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledVehicle.h"

namespace v3d {
class Physics;
class Vehicle;

using VehicleRaw_ptr = object_ptr<std::vector<chrono::vehicle::WheeledVehicle>, chrono::vehicle::WheeledVehicle, size_t>;

struct VehicleInputs {
    // chrono::vehicle::WheeledVehicle* vehicle;
    VehicleRaw_ptr vehicle;
    chrono::vehicle::DriverInputs driverInputs;
};

// class VehicleData {
//     friend Physics;
//     friend Vehicle;
//    private:
//     chrono::vehicle::WheeledVehicle m_rawVehicle;
//     chrono::vehicle::DriverInputs m_driverInputs;

//    public:
//     VehicleData() = delete;
//     VehicleData(chrono::ChSystemNSC *systemNSC, std::string vehicleModelPath) {
//         m_rawVehicle = chrono::vehicle::WheeledVehicle(systemNSC, vehicleModelPath, true, true);
//         m_driverInputs = chrono::vehicle::DriverInputs();
//     };

//     void setDriverInputs(chrono::vehicle::DriverInputs driverInputs) { m_driverInputs = driverInputs; }
//     chrono::vehicle::DriverInputs getDriverInputs() { return m_driverInputs; }
// }

using VehicleHandle = object_ptr<std::vector<VehicleInputs>, VehicleInputs, size_t>;
}  // namespace v3d
