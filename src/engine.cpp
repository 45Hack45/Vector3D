#include "engine.h"

#include <plog/Log.h>

#include <atomic>
#include <boost/stacktrace.hpp>
#include <cassert>
#include <csignal>
#include <ostream>
#include <sstream>
#include <string>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "input/KeyboardDevice.hpp"
#include "physics/Vehicle.h"
#include "physics/VehicleInteractiveController.h"
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

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void initImgui(GLFWwindow* window, float mainScale, bool darkMode = true) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    if (darkMode)
        ImGui::StyleColorsDark();
    else
        ImGui::StyleColorsClassic();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(
        mainScale);  // Bake a fixed style scale. (until we have a solution for
                     // dynamic style scaling, changing this requires resetting
                     // Style + calling this again)
    style.FontScaleDpi =
        mainScale;  // Set initial font scale. (using
                    // io.ConfigDpiScaleFonts=true makes this unnecessary. We
                    // leave both here for documentation purpose)

    const char* glsl_version = "#version 130";

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void imgui_beginFrame_() {
    // GUI window
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_RenderFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

namespace v3d {
void Engine::init() {
    if (m_initialized) {
        throw std::runtime_error("Engine initialized multiple times");
    }

    // Initialize signal handler to log unhandled errors and other signals
    initSignalHandler();

    PLOGI << "Initializing Engine" << std::endl;

    // Initialize GLFW and create a window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) throw std::runtime_error("Failed initializing GLFW!");

    float mainScale =
        ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    switch (m_gBackendType) {
        case v3d::rendering::GraphicsBackendType::NONE:
            m_window->init("Vector3D Headless",
                           rendering::WindowBackendHint::NONE, mainScale);

            m_nullGraphicsBackend =
                new rendering::NullGraphicsBackend(m_window);
            m_graphicsBackend = m_nullGraphicsBackend;
            break;
        case rendering::GraphicsBackendType::VULKAN_API:
            m_window->init("Vector3D Vulkan",
                           rendering::WindowBackendHint::VULKAN_API, mainScale);

            m_vulkanBackend = new rendering::VulkanBackend(m_window);
            m_graphicsBackend = m_vulkanBackend;
            // m_vulkanBackend->init();
            break;
        case rendering::GraphicsBackendType::OPENGL_API:
            m_window->init("Vector3D OpenGL",
                           rendering::WindowBackendHint::OPENGL_API, mainScale);

            m_openGlBackend = new rendering::OpenGlBackend(m_window);
            m_graphicsBackend = m_openGlBackend;
            break;
        default:
            throw std::runtime_error(
                "Failed to initialize engine, invalid graphics backend type!");
            break;
    }

    m_graphicsBackend->init();

    initImgui(m_window->getWindow(), mainScale, true);

    m_editor = editor::Editor::Instance();
    m_editor->Init(this);

    // Instantiate default keyboard device and mappings
    initDefaultInput();

    m_initialized = true;

    // Initialize the scene and add entities
    m_scene = Scene::create(this, &m_phSystem);
    // auto entity = m_scene->instantiateEntity("Test object");
    // auto entity2 = m_scene->instantiateEntity("Test child object", entity);
    // auto entity3 = m_scene->instantiateEntity("Test grandchild object",
    // entity2); auto entity4 = m_scene->instantiateEntity("Test object 2");
    // auto entity_car = m_scene->instantiateEntity("Brum brum");

    // Mesh* mesh =
    // m_graphicsBackend->createMesh("resources/primitives/3D/bunny.obj"); Mesh*
    // carMesh =
    // m_graphicsBackend->createMesh("resources/test_models/beetle-alt.obj");
    Mesh* carMesh = m_graphicsBackend->createMesh(
        "resources/vehicle_model/sedan/sedan_chassis_col.obj");

    auto ground = m_scene->instantiateEntity("Ground");
    auto groundRigidBody = m_scene->getComponentOfType<RigidBody>(ground);
    auto groundTransform = m_scene->getComponentOfType<Transform>(ground);
    auto groundCollider =
        m_scene->createEntityComponentOfType<ColliderBox>(ground);
    groundRigidBody->setFixed(true);
    groundRigidBody->setPos(0, -1, 0);
    groundCollider->setSize(1, .1, 1);
    groundTransform->setScale(1, .1, 1);
    auto groundRenderer =
        m_scene->createEntityComponentOfType<MeshRenderer>(ground);
    groundRenderer->setMesh(m_graphicsBackend->m_primitives.m_cube);

    // auto bunny = m_scene->instantiateEntity("Bunny");
    // auto bunnyCollider =
    // m_scene->createEntityComponentOfType<ColliderBox>(bunny); auto
    // bunnyTransform = m_scene->getComponentOfType<Transform>(bunny); auto
    // bunnyRigidBody = m_scene->getComponentOfType<RigidBody>(bunny);
    // // bunnyRigidBody->setFixed(true);
    // bunnyRigidBody->setVelocity(chrono::ChVector3d(0, 0, 0));
    // bunnyRigidBody->setPos(0, 1, 0);
    // bunnyCollider->setSize(1, 1, 1);
    // bunnyTransform->setScale(10, 10, 10);
    // auto bunnyRenderer =
    // m_scene->createEntityComponentOfType<MeshRenderer>(bunny);
    // bunnyRenderer->setMesh(mesh);

    int num_vehicles = 1;
    float separation = 1;

    for (int i = 0; i < num_vehicles; i++) {
        auto vehicle = m_scene->instantiateEntity("WheeledVehicle");
        auto vehicleComponent =
            m_scene->createEntityComponentOfType<Vehicle>(vehicle);
        vehicleComponent->setFilePath(std::string(
            "resources/vehicle_model/sedan/vehicle/Sedan_Vehicle.json"));
        auto vehicleRenderer =
            m_scene->createEntityComponentOfType<MeshRenderer>(vehicle);
        vehicleRenderer->setMesh(carMesh);

        float angle = (float(i) / float(num_vehicles)) * 360.0f;
        chrono::ChVector3d position =
            chrono::ChVector3d(sin(angle), .5, cos(angle)) * separation;
        vehicleComponent->setInitialPosition(position);

        // Add controller
        auto VehicleController =
            m_scene->createEntityComponentOfType<VehicleInteractiveController>(
                vehicle);
    }

    m_scene->print_entities();
}

void Engine::start() {
    m_scene->m_components.for_each(
        [](ComponentBase& component) { component.start(); });
}

void Engine::cleanup() {
    if (!m_initialized) {
        return;
    }

    // Imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_graphicsBackend->cleanup();
    m_window->cleanup();
    glfwTerminate();

    m_initialized = false;
}

void Engine::mainLoop() {
    bool running = !recieved_forced_close_signal;

    std::cout << "Initial Frame count " << m_last_frame_dt.count() << "\n";

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Main game loop
    while (running) {
        running = !recieved_forced_close_signal && !m_window->shouldClose();

        const auto frame_start = std::chrono::steady_clock::now();

        // Poll for window events
        m_window->pollEvents();
        if (glfwGetWindowAttrib(m_window->getWindow(), GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        assert(m_last_frame_dt.count() >= 0 &&
               std::isfinite(m_last_frame_dt.count()));

        if (io.WantCaptureMouse or io.WantCaptureKeyboard) {
            m_inputManager.muteInput(true);
        } else {
            m_inputManager.muteInput(false);
            processInput(m_window->getWindow());
        }

        // Start the Dear ImGui frame
        imgui_beginFrame_();

        // Update logic
        m_scene->update(m_last_frame_dt.count());

        // Update Physics
        for (int i = 0; i < 16; i++) m_phSystem.stepSimulation();

        // Render frame
        // TODO: Pass time and dt, to be able to pass them to a shader
        m_graphicsBackend->update();

        // Render GUI
        m_editor->renderGui(m_last_frame_dt.count(), &m_scene->m_root.get(),
                            m_scene.get());

        // Render Imgui UI
        imgui_RenderFrame();

        // Swap buffer
        m_graphicsBackend->present();

        // Enforce physics soft-realtime
        m_phSystem.spin(.016666667);  // Target 60 fps

        // Update deltatime
        const auto frame_end = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = frame_end - frame_start;
        m_last_frame_dt = diff;
        // std::cout << "Last frame dt (ms): " <<
        // std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
        // << "\n";
    }
}

void Engine::initDefaultInput() {
    input::InputProfile keyboardProfile;
    keyboardProfile.bind(input::action::IAct_Accelerate, input::key::IK_I);
    keyboardProfile.bind(input::action::IAct_Back, input::key::IK_K);
    keyboardProfile.bind(input::action::IAct_Break, input::key::IK_SPACE);
    keyboardProfile.bind(input::action::IAct_SteerLeft, input::key::IK_J);
    keyboardProfile.bind(input::action::IAct_SteerRight, input::key::IK_L);
    keyboardProfile.bind(input::action::IAct_Clutch, input::key::IK_C);

    m_inputManager.addDevice(
        std::make_unique<input::KeyboardDevice>(m_window, keyboardProfile));
}

void Engine::processInput(GLFWwindow* window) {}

}  // namespace v3d
