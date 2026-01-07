#pragma once

#include <engine.h>

#include "demos/demo_common.h"
#include "editor/ComponentRegistry.h"
#include "physics/Vehicle.h"
#include "physics/VehicleInteractiveController.h"
#include "physics/rigidbody.h"
#include "rendering/mesh_renderer.h"
#include "transform.h"

namespace v3d {
namespace demos {
class DemoSedanVehicle : public v3d::Engine {
   private:
   protected:
    AssimpLoader* m_modelLoader = nullptr;

    std::unique_ptr<ModelLoader> makeModelLoader() override {
        // Init Model manager with Assimp as loader
        Assimp::DefaultLogger::create("AssimpLog.txt",
                                      Assimp::Logger::LogSeverity::NORMAL);
        AssimpLoader::attachDefaultLogger();

        AssimpLoaderPropertes properties;
        auto assimpLoader =
            std::make_unique<AssimpLoader>(std::move(properties));
        m_modelLoader = assimpLoader.get();
        return assimpLoader;
    }

    void engineStart() override {
        auto testEntity = m_scene->instantiateEntity("Test entity");
        // auto testEntityComponent =
        //     m_scene->createEntityComponentOfType<RigidBody>(testEntity);

        // std::cout << "--------------- Test "
        //           //   << testEntityComponent->m_registrar.m_componentName
        //           << RigidBody::m_registrar.m_componentName << "
        //           ---------\n";

        auto porscheRootEntity =
            m_scene->instantiateEntity("Porsche_911_GT2.obj");
        auto porscheVehicleComponent =
            m_scene->createEntityComponentOfType<Vehicle>(porscheRootEntity);
        porscheVehicleComponent->setFilePath(std::string(
            "resources/vehicle_model/sedan/vehicle/Sedan_Vehicle.json"));
        porscheVehicleComponent->start();
        // Add controller
        auto porscheVehicleController =
            m_scene->createEntityComponentOfType<VehicleInteractiveController>(
                porscheRootEntity);

        // // TODO: Unify loader properties, to be able to set properties
        // // regardless of specific loader
        // // auto modelpath =
        // //     "resources/test_models/p911GT/"
        // //     "Porsche_911_GT2_fixed.obj";
        auto modelpath =
            "resources/vehicle_model/sedan/"
            "sedan_chassis_vis_fix.obj";
        Model* porscheModel = m_modelManager->importModel<MeshOpenGL>(
            modelpath, "Porsche 911 GT2");

        for (auto mesh : porscheModel->getMeshes()) {
            auto porscheEntity =
                m_scene->instantiateEntity(std::string(mesh->getName()));
            auto porscheRigidBody =
                m_scene->getComponentOfType<RigidBody>(porscheEntity);
            porscheRigidBody->setFixed(false);
            auto porscheTransform =
                m_scene->getComponentOfType<Transform>(porscheEntity);
            auto porscheRenderer =
                m_scene->createEntityComponentOfType<MeshRenderer>(
                    porscheEntity);
            porscheRenderer->setMesh(mesh);

            porscheEntity->setParent(porscheRootEntity);
        }

        auto chasissPath =
            "resources/vehicle_model/sedan/"
            "sedan_chassis_vis_fix.obj";
        auto rimPath = "resources/vehicle_model/sedan/sedan_rim.obj";
        auto tirePath = "resources/vehicle_model/sedan/sedan_tire.obj";
    }

   public:
    DemoSedanVehicle() = delete;
    DemoSedanVehicle(uint32_t width, uint32_t height,
                     rendering::GraphicsBackendType graphicsBackendType)
        : v3d::Engine(width, height, graphicsBackendType) {};
    ~DemoSedanVehicle() override {};
};

}  // namespace demos

}  // namespace v3d

int demoSedanVehicle(uint32_t width, uint32_t height,
                     v3d::rendering::GraphicsBackendType graphicsBackendType) {
    const char* DEMO_LOG_START_MESSAGE = R"(
.--------------------------------------------------------------.
|                        Starting Demo: Sedan Vehicle          |
'--------------------------------------------------------------'
    )";
    PLOGN << DEMO_LOG_START_MESSAGE;

    try {
        {
            // Create the 3D engine instance
            v3d::demos::DemoSedanVehicle demo(width, height,
                                              graphicsBackendType);

            // Initialize and run the 3D engine
            demo.run();
        }
    } catch (const std::exception& e) {
        // std::cerr << e.what() << std::endl;
        PLOGE << "Exception: " << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
