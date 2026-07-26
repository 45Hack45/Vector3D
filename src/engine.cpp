#include "engine.h"

#include <ModelLoader.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <plog/Log.h>

#include <atomic>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/stacktrace.hpp>
#include <cassert>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "input/KeyboardDevice.h"
#include "physics/Vehicle.h"
#include "physics/VehicleInteractiveController.h"
#include "physics/collider.h"
#include "physics/rigidbody.h"
#include "plog/Severity.h"
#include "rendering/mesh_renderer.h"
#include "rendering/null_graphics_backend.hpp"
#include "serialization.hpp"
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
    PLOG(severity) << "Signal (" << signal_str << ") recieved:\n" << msg_ss.str() << "\n";
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport /
    //     Platform
    //                                        // Windows

    // Setup Dear ImGui style
    if (darkMode)
        ImGui::StyleColorsDark();
    else
        ImGui::StyleColorsClassic();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);  // Bake a fixed style scale. (until we have a solution for
                                     // dynamic style scaling, changing this requires resetting
                                     // Style + calling this again)
    style.FontScaleDpi = mainScale;  // Set initial font scale. (using
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

#ifndef NDEBUG
// Load counter for [GIZMOS.*] diagnostics; 0 = pre-first-load.
static int s_gizmosLoadGeneration = 0;
#endif

Engine::Engine(uint32_t width, uint32_t height,
               rendering::GraphicsBackendType graphicsBackendType) {
    m_engineStartTime = std::chrono::steady_clock::now();

    // Initialize signal handler to log unhandled errors and other signals
    initSignalHandler();

    PLOGI << "Initializing Engine" << std::endl;

    // Initialize GLFW and create a window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) throw std::runtime_error("Failed initializing GLFW!");

    m_gBackendType = graphicsBackendType;
    float mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    switch (m_gBackendType) {
        case v3d::rendering::GraphicsBackendType::NONE:
            m_window =
                std::make_unique<Window>("Vector3D Headless", rendering::WindowBackendHint::NONE,
                                         width, height, mainScale, true);

            m_graphicsBackend = std::make_unique<rendering::NullGraphicsBackend>(m_window.get());
            break;
        case rendering::GraphicsBackendType::VULKAN_API:
            m_window = std::make_unique<Window>("Vector3D Vulkan",
                                                rendering::WindowBackendHint::VULKAN_API, width,
                                                height, mainScale, true);

            m_graphicsBackend = std::make_unique<rendering::VulkanBackend>(m_window.get());
            break;
        case rendering::GraphicsBackendType::OPENGL_API:
            m_window = std::make_unique<Window>("Vector3D OpenGL",
                                                rendering::WindowBackendHint::OPENGL_API, width,
                                                height, mainScale, true);

            m_graphicsBackend = std::make_unique<rendering::OpenGlBackend>(m_window.get());
            break;
        default:
            throw std::runtime_error("Failed to initialize engine, invalid graphics backend type!");
            break;
    }

    initImgui(m_window->getWindow(), mainScale, true);

    m_componentRegistry = &ComponentRegistry::instance();
    m_editor = std::make_unique<editor::Editor>(this);

    // Init Model loader and manager
    std::unique_ptr<ModelLoader> modelLoader = makeModelLoader();
    m_modelManager = std::make_unique<ModelManager>(std::move(modelLoader));

    // Instantiate default keyboard device and mappings
    initDefaultInput();

    PLOGI << "Engine Initialized" << std::endl;

    PLOGV << "Initializing Scene" << std::endl;
    // Initialize the scene and add entities
    m_scene = Scene::create(this, &m_phSystem);

#ifndef NDEBUG
    PLOGV << "[GIZMOS.count] gen=" << s_gizmosLoadGeneration
          << " point=startup-after-Scene::init count=" << m_graphicsBackend->gizmosTargetCount();
#endif

    // Pre-initialize scene component vectors, required to be able to add
    // components dynamically (not known at compile-time, for example adding a
    // component through the GUI)
    registerComponents(m_scene.get(), m_componentRegistry);
    PLOGV << "Scene Initialized" << std::endl;

    m_scene->printEntities();
};

Engine::~Engine() {
    m_scene.reset();

    // Clear managers resources
    m_modelManager.reset();

    // Clenup assimp logger
    Assimp::DefaultLogger::kill();

    m_editor.reset();

    // Imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_graphicsBackend.reset();
    m_window.reset();
    glfwTerminate();
}

void Engine::start() {
    engineStartPre();
    m_scene->m_components.for_each([](ComponentBase& component) { component.start(); });
    engineStart();
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
        double last_frame_dt = m_last_frame_dt.count();

        // Poll for window events
        m_window->pollEvents();
        if (glfwGetWindowAttrib(m_window->getWindow(), GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        assert(last_frame_dt >= 0 && std::isfinite(last_frame_dt));

        if (io.WantCaptureMouse or io.WantCaptureKeyboard) {
            m_inputManager.muteInput(true);
        } else {
            m_inputManager.muteInput(false);
            processInput(m_window->getWindow());
        }

        // Must run before anything reads input: refreshes cached device state
        // and the gamepad list.
        m_inputManager.update();

        // Start the Dear ImGui frame
        imgui_beginFrame_();

        // Update logic
        logicFrameUpdatePre(last_frame_dt);
        m_scene->update(last_frame_dt);
        logicFrameUpdate(last_frame_dt);

        // Update Physics
        physicsFrameUpdatePre();
        for (int i = 0; i < m_phSystem.getStepPerFrame(); i++) m_phSystem.stepSimulation();
        physicsFrameUpdate();

        // Render frame
        // TODO: Pass time and dt, to be able to pass them to a shader
        graphicsFrameUpdatePre();
        m_graphicsBackend->update();
        graphicsFrameUpdate();

        // Render GUI
        editorGUIFrameUpdatePre();
        m_editor->renderGui(last_frame_dt, m_scene->m_root,
                            m_scene.get());
        editorGUIFrameUpdate();

        // Render debbug window
        renderEngineDebugGui(last_frame_dt);

        // Render Imgui UI
        imgui_RenderFrame();

        // Swap buffer
        m_graphicsBackend->present();

        // Enforce physics soft-realtime
        m_phSystem.spin(1.f / (float)m_targetFrameRate);  // Target 60 fps

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
    // Register before loading so the built-in bindings resolve on first apply.
    input::registerBuiltinActions();

    // Hotplug-created gamepads need the window they will poll.
    m_inputManager.setWindow(m_window.get());

    // Defaults go through the store so the first save writes them out.
    input::DeviceConfig keyboard;
    keyboard.deviceKind = input::deviceKindName(input::InputDeviceType::Keyboard);
    keyboard.guid = std::string(input::kKeyboardDeviceGuid);
    keyboard.lastKnownName = std::string(input::kKeyboardDeviceName);
    keyboard.activeProfile = std::string(input::kDefaultProfileName);
    keyboard.profiles = {
        input::DeviceProfile{std::string(input::kDefaultProfileName),
                          {
                              {"Accelerate", "I"},
                              {"Back", "K"},
                              {"Brake", "SPACE"},
                              {"SteerLeft", "J"},
                              {"SteerRight", "L"},
                              {"Clutch", "C"},
                          }},
        input::DeviceProfile{"WASD",
                          {
                              {"Accelerate", "W"},
                              {"Back", "S"},
                              {"Brake", "SPACE"},
                              {"SteerLeft", "A"},
                              {"SteerRight", "D"},
                              {"Clutch", "LEFT_SHIFT"},
                          }},
    };
    m_inputManager.setDeviceConfig(std::move(keyboard));

    m_inputManager.addDevice(std::make_unique<input::KeyboardDevice>(m_window.get()));

    // No config is the normal first run; the built-in defaults stand.
    const std::filesystem::path configPath = InputManager::defaultConfigPath();
    if (std::filesystem::exists(configPath)) {
        const input::InputConfigResult result = m_inputManager.loadConfig(configPath);
        if (result.ok) {
            PLOGI << "Input config: " << result.message;
        } else {
            PLOGW << "Input config: " << result.message;
        }
    } else {
        PLOGI << "Input config: no config at " << configPath.string()
              << ", using built-in defaults";
    }
}

void Engine::processInput(GLFWwindow* window) {}

void Engine::renderEngineDebugGui(double delta) {
    ImGui::Begin("Debbug");
    int targetFPS = m_targetFrameRate;
    if (ImGui::InputInt("Target FPS", &targetFPS, 1, 10)) {
        m_targetFrameRate = targetFPS;
    }
    ImGui::Spacing();

    if (ImGui::Button("Test save scene")) {
        saveScene("testSerialization/testSceneSave.xml");
    }
    if (ImGui::Button("Test load scene")) {
        loadScene("testSerialization/testSceneSave.xml");
    }

    ImGui::Spacing();
    if (ImGui::Button("Test save scene XML")) {
        saveScene("testSerialization/testSceneSave.xml", true);
    }
    if (ImGui::Button("Test load scene XML")) {
        loadScene("testSerialization/testSceneSave.xml", true);
    }

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Physics")) m_phSystem.renderDebbugGUI();
    ImGui::Spacing();

    ImGui::End();
}

void Engine::registerComponents(Scene* scene, ComponentRegistry* componentRegistry) {
    std::vector<const ComponentRegistrationInfo*> componentsInfo = componentRegistry->getAllInfo();
    for (auto info : componentsInfo) {
        scene->m_components.registerType(info->componentType, info->componentCollectionFactory(),
                                         info->name);
    }
}

void Engine::saveScene(std::string filename, bool xml) {
    std::ofstream ofs(filename);
    assert(ofs.good());

    if (xml) {
        boost::archive::xml_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("scene", m_scene);
    } else {
        boost::archive::text_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("scene", m_scene);
    }
}

void Engine::loadScene(std::string filename, bool xml) {
#ifndef NDEBUG
    // Gizmos count checkpoint: loadScene entry. The generation increments
    // once per call; the same value tags the later checkpoints of this load.
    ++s_gizmosLoadGeneration;
    PLOGV << "[GIZMOS.count] gen=" << s_gizmosLoadGeneration
          << " point=loadScene-entry count=" << m_graphicsBackend->gizmosTargetCount();
#endif

    std::ifstream ifs(filename);
    assert(ifs.good());
    assert(ifs.is_open());
    if (!ifs.is_open()) std::cout << "failed to open " << filename << '\n';

    std::shared_ptr<Scene> scene;

    if (xml) {
        boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("scene", scene);
    } else {
        boost::archive::text_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("scene", scene);
    }

    if (scene) {
        scene->m_engine = this;
        scene->m_phSystem = &m_phSystem;

        // Reset editor selection before the selected element is destroyed
        m_editor->selected = nullptr;

        // Destroy the current scene before initialising the new one
        m_scene.reset();

#ifndef NDEBUG
        PLOGV << "[GIZMOS.count] gen=" << s_gizmosLoadGeneration
              << " point=after-scene-reset count=" << m_graphicsBackend->gizmosTargetCount();
        m_graphicsBackend->debugDumpGizmosTargets("after-old-scene-destroyed");
#endif

        // Clear accumulated vehicle state that Physics owns across loads.
        // TODO: This leaks. WheeledVehicle::Initialize registered the chassis,
        // spindles, links and shafts in the chrono system, destroying the WheeledVehicle does not
        // remove them. Every load leaves behind a ghost vehicle that keeps simulating. Chrono has
        // no vehicle teardown, so the parts have to be removed one by one
        m_phSystem.clearVehicles();

        // Now safe to add new bodies and vehicles.
        scene->onLoad();

#ifndef NDEBUG
        PLOGV << "[GIZMOS.count] gen=" << s_gizmosLoadGeneration
              << " point=after-onLoad count=" << m_graphicsBackend->gizmosTargetCount();
#endif

        scene->printEntities();
        m_scene = scene;

#ifndef NDEBUG
        // gizmos target count must match live component count after every load.
        size_t liveComponents = 0;
        m_scene->m_components.for_each([&](ComponentBase&) { liveComponents++; });
        size_t gizmosTargets = m_graphicsBackend->gizmosTargetCount();
        PLOGV << "[GIZMOS.check] gen=" << s_gizmosLoadGeneration
              << " liveComponents=" << liveComponents << " gizmosTargets=" << gizmosTargets
              << (gizmosTargets == liveComponents ? " OK" : " MISMATCH");
        assert(gizmosTargets == liveComponents &&
               "gizmos target count diverged from live component count after load");
#endif
    } else {
        // TODO: Improve error
        throw std::runtime_error("Failed to load scene");
    }
}

}  // namespace v3d
