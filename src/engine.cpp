#include "engine.h"

#include <plog/Log.h>

#include <atomic>
#include <boost/stacktrace.hpp>
#include <cassert>
#include <csignal>
#include <ostream>
#include <sstream>
#include <string>

#include "physics/Vehicle.h"
#include "physics/collider.h"
#include "physics/rigidbody.h"
#include "plog/Severity.h"
#include "rendering/mesh_renderer.h"
#include "rendering/null_graphics_backend.hpp"
#include "transform.h"

// ------------------------------- TEMP ----------------------------------
#include "chrono_vehicle/ChConfigVehicle.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/terrain/FlatTerrain.h"
#include "chrono_vehicle/terrain/RigidTerrain.h"
#include "chrono_vehicle/utils/ChUtilsJSON.h"
// #include "chrono/utils/ChUtils.h"

#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledTrailer.h"
#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledVehicle.h"
// ----------------------------------------------------------------------

std::atomic<bool> recieved_forced_close_signal{false};

void signalHandler(int signal) {
    std::string signal_str = std::to_string(signal);
    std::stringstream msg_ss;
    plog::Severity severity = plog::verbose;

    switch (signal) {
        case SIGTERM:
            signal_str = "SIGTERM";
            break;
        case SIGINT:
            signal_str = "SIGINT";
            break;
        case SIGSEGV:
            signal_str = "SIGSEGV";
            break;
        case SIGABRT:
            signal_str = "SIGABRT";
            std::_Exit(EXIT_FAILURE);  // exit immediately
            break;
        case SIGILL:
            signal_str = "SIGILL";
            break;
        case SIGFPE:
            signal_str = "SIGFPE";
            break;
    }
    msg_ss << boost::stacktrace::stacktrace() << "\n";
    msg_ss << "Exiting...\n";
    recieved_forced_close_signal = true;
    PLOG(severity) << "Signal (" << signal_str << ") recieved:\n"
                   << msg_ss.str() << "\n";
    std::_Exit(EXIT_FAILURE);  // exit immediately
}

void initSignalHandler() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    // std::signal(SIGABRT, signalHandler);
    std::signal(SIGILL, signalHandler);
    std::signal(SIGFPE, signalHandler);
}

chrono::vehicle::WheeledVehicle* m_vehicle;

namespace v3d {
void Engine::init() {
    if (m_initialized) {
        throw std::runtime_error("Engine initialized multiple times");
    }

    // Initialize signal handler to log unhandled errors and other signals
    initSignalHandler();

    PLOGI << "Initializing Engine" << std::endl;

    // Initialize GLFW and create a window
    glfwInit();
    switch (m_gBackendType) {
        case v3d::rendering::GraphicsBackendType::NONE:
            m_window->init("Vector3D", rendering::WindowBackendHint::NONE);

            m_nullGraphicsBackend = new rendering::NullGraphicsBackend(m_window);
            m_graphicsBackend = m_nullGraphicsBackend;
            break;
        case rendering::GraphicsBackendType::VULKAN_API:
            m_window->init("Vector3D", rendering::WindowBackendHint::VULKAN_API);

            m_vulkanBackend = new rendering::VulkanBackend(m_window);
            m_graphicsBackend = m_vulkanBackend;
            // m_vulkanBackend->init();
            break;
        case rendering::GraphicsBackendType::OPENGL_API:
            m_window->init("Vector3D", rendering::WindowBackendHint::OPENGL_API);

            m_openGlBackend = new rendering::OpenGlBackend(m_window);
            m_graphicsBackend = m_openGlBackend;
            break;
        default:
            throw std::runtime_error("Failed to initialize engine, invalid graphics backend type!");
            break;
    }

    m_graphicsBackend->init();

    m_initialized = true;

    // Initialize the scene and add entities
    m_scene = Scene::create(this, &m_phSystem);
    // auto entity = m_scene->instantiateEntity("Test object");
    // auto entity2 = m_scene->instantiateEntity("Test child object", entity);
    // auto entity3 = m_scene->instantiateEntity("Test grandchild object", entity2);
    // auto entity4 = m_scene->instantiateEntity("Test object 2");
    // auto entity_car = m_scene->instantiateEntity("Brum brum");

    // Mesh* mesh = m_graphicsBackend->createMesh("resources/primitives/3D/bunny.obj");
    // Mesh* carMesh = m_graphicsBackend->createMesh("resources/test_models/beetle-alt.obj");
    Mesh* carMesh = m_graphicsBackend->createMesh("resources/vehicle_model/sedan/sedan_chassis_col.obj");

    auto ground = m_scene->instantiateEntity("Ground");
    auto groundRigidBody = m_scene->getComponentOfType<RigidBody>(ground);
    auto groundTransform = m_scene->getComponentOfType<Transform>(ground);
    auto groundCollider = m_scene->createEntityComponentOfType<ColliderBox>(ground);
    groundRigidBody->setFixed(false);
    groundRigidBody->setPos(0, -1, 0);
    groundCollider->setSize(1, .1, 1);
    groundTransform->setScale(1, .1, 1);
    auto groundRenderer = m_scene->createEntityComponentOfType<MeshRenderer>(ground);
    groundRenderer->setMesh(m_graphicsBackend->m_primitives.m_cube);

    // auto bunny = m_scene->instantiateEntity("Bunny");
    // auto bunnyCollider = m_scene->createEntityComponentOfType<ColliderBox>(bunny);
    // auto bunnyTransform = m_scene->getComponentOfType<Transform>(bunny);
    // auto bunnyRigidBody = m_scene->getComponentOfType<RigidBody>(bunny);
    // // bunnyRigidBody->setFixed(true);
    // bunnyRigidBody->setVelocity(chrono::ChVector3d(0, 0, 0));
    // bunnyRigidBody->setPos(0, 1, 0);
    // bunnyCollider->setSize(1, 1, 1);
    // bunnyTransform->setScale(10, 10, 10);
    // auto bunnyRenderer = m_scene->createEntityComponentOfType<MeshRenderer>(bunny);
    // bunnyRenderer->setMesh(mesh);

    // // ------------------------------- TEMP ----------------------------------
    // // Initial vehicle position and orientation (adjust for selected terrain)
    // chrono::ChVector3d initLoc(0, 0, 0.5);
    // chrono::ChQuaterniond initRot(1, 0, 0, 0);

    // // "resources/vehicle_model/sedan/vehicle/Sedan_Vehicle.json"

    // m_vehicle = new chrono::vehicle::WheeledVehicle(&m_phSystem.m_system, "resources/vehicle_model/sedan/vehicle/Sedan_Vehicle.json", true, true);
    // m_vehicle->Initialize(chrono::ChCoordsys<>(initLoc, initRot));
    // // m_vehicle->GetChassis()->SetFixed(false);
    // // ------------------------------- TEMP ----------------------------------

    int num_vehicles = 1;
    float separation = 1;

    for (int i = 0; i < num_vehicles; i++) {
        auto vehicle = m_scene->instantiateEntity("WheeledVehicle");
        auto vehicleComponent = m_scene->createEntityComponentOfType<Vehicle>(vehicle);
        vehicleComponent->setFilePath(std::string("resources/vehicle_model/sedan/vehicle/Sedan_Vehicle.json"));
        auto vehicleRenderer = m_scene->createEntityComponentOfType<MeshRenderer>(vehicle);
        vehicleRenderer->setMesh(carMesh);

        float angle = (float(i) / float(num_vehicles)) * 360.0f;
        chrono::ChVector3d position = chrono::ChVector3d(sin(angle), .5, cos(angle)) * separation;
        vehicleComponent->setInitialPosition(position);
    }

    m_scene->print_entities();
}

void Engine::start() {
    m_scene->m_components.for_each([](ComponentBase& component) { component.start(); });
}

void Engine::cleanup() {
    if (!m_initialized) {
        return;
    }
    m_graphicsBackend->cleanup();
    m_window->cleanup();
    glfwTerminate();

    m_initialized = false;
}

void Engine::mainLoop() {
    bool running = !recieved_forced_close_signal;

    std::cout << "Initial Frame count " << m_last_frame_dt.count() << "\n";

    // Main game loop
    while (running) {
        running = !recieved_forced_close_signal && !m_window->shouldClose();

        const auto frame_start = std::chrono::steady_clock::now();

        // Poll for window events
        m_window->pollEvents();

        assert(m_last_frame_dt.count() >= 0 && std::isfinite(m_last_frame_dt.count()));

        processInput(window);

        // Update logic
        m_scene->update(m_last_frame_dt.count());

        // Update Physics
        m_phSystem.stepSimulation();
        m_phSystem.stepSimulation();
        m_phSystem.stepSimulation();
        m_phSystem.stepSimulation();

        m_phSystem.stepSimulation();
        m_phSystem.stepSimulation();
        m_phSystem.stepSimulation();
        m_phSystem.stepSimulation();
        // std::cout << "Vehicle pos " << m_vehicle->GetChassis()->GetPos() << "\n";
        // running = false;

        // Render frame
        // TODO: Pass time and dt, to be able to pass them to a shader
        m_graphicsBackend->update();

        // Enforce physics soft-realtime
        // m_phSystem.spin(1 / 60);  // Target 60 fps

        // Update deltatime
        const auto frame_end = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = frame_end - frame_start;
        m_last_frame_dt = diff;
        // std::cout << "Last frame dt (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "\n";
    }
}

void Engine::processInput(GLFWwindow* window) {

    auto vehicle = m_scene->getComponentOfType<Vehicle>();

    if (vehicle != nullptr){
        vehicle->setThrottle(glfwGetKey(m_window->getWindow(), GLFW_KEY_I) == GLFW_PRESS? 1.0: 0.0);
        vehicle->setBraking(glfwGetKey(m_window->getWindow(), GLFW_KEY_K) == GLFW_PRESS? 1.0: 0.0);
        float steering = glfwGetKey(m_window->getWindow(), GLFW_KEY_J) == GLFW_PRESS? 1.0: 0.0;
        steering -= glfwGetKey(m_window->getWindow(), GLFW_KEY_L) == GLFW_PRESS? 1.0: 0.0;
        vehicle->setSteering(steering);

    }
}

}  // namespace v3d
