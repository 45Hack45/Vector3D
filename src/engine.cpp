#include "engine.h"

#include <plog/Log.h>

#include <atomic>
#include <boost/stacktrace.hpp>
#include <csignal>
#include <ostream>
#include <sstream>
#include <string>

#include "physics/collider.h"
#include "physics/rigidbody.h"
#include "plog/Severity.h"
#include "rendering/mesh_renderer.h"
#include "rendering/null_graphics_backend.hpp"
#include "transform.h"

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
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGILL, signalHandler);
    std::signal(SIGFPE, signalHandler);
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

    Mesh* mesh = m_graphicsBackend->createMesh("EngineProject/stanford-bunny.obj");

    auto ground = m_scene->instantiateEntity("Ground");
    auto groundRigidBody = m_scene->getComponentOfType<RigidBody>(ground);
    auto groundCollider = m_scene->createEntityComponentOfType<ColliderBox>(ground);
    groundRigidBody->setFixed(true);
    groundRigidBody->setPos(0, -1, 0);
    groundCollider->setSize(10, .2, 10);
    auto groundRenderer = m_scene->createEntityComponentOfType<MeshRenderer>(ground);
    groundRenderer->setMesh(mesh);

    auto bunny = m_scene->instantiateEntity("Bunny");

    auto bunnyCollider_id = m_scene->instantiateEntityComponent<ColliderBox>(bunny);
    componentID_t renderer_id = m_scene->instantiateEntityComponent<MeshRenderer>(bunny);
    MeshRenderer* renderer = m_scene->getComponent<MeshRenderer>(renderer_id);
    renderer->setMesh(mesh);

    m_scene->print_entities();
}

void Engine::start() {
    int delta = 0;
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

    // Main game loop
    while (running) {
        running = !recieved_forced_close_signal && !m_window->shouldClose();

        const auto frame_start = std::chrono::steady_clock::now();

        // Poll for window events
        m_window->pollEvents();

        // Update logic
        m_scene->update(m_last_frame_dt.count());

        // Update Physics
        m_phSystem.m_system.DoStepDynamics(m_last_frame_dt.count());

        // Render frame
        m_graphicsBackend->frame_update();

        // Update deltatime
        const auto frame_end = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = frame_end - frame_start;
        m_last_frame_dt = diff;
        // std::cout << "Last frame dt (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "\n";
    }
}
}  // namespace v3d
