#include "engine.h"
#include "rendering/null_graphics_backend.hpp"
#include <plog/Log.h>

namespace v3d {
    void Engine::init() {
        if (m_initialized) {
            throw std::runtime_error("Engine initialized multiple times");
        }

        PLOGI << "Initializing Engine" << std::endl;

        // Initialize GLFW and create a window
        switch (m_gBackendType)
        {
            case v3d::rendering::GraphicsBackendType::NONE:
            m_window->init("Vector3D", rendering::WindowBackendHint::NONE);

            m_nullGraphicsBackend = new rendering::NullGraphicsBackend(m_window);            
            m_graphicsBackend = m_nullGraphicsBackend;
            break;
        case rendering::GraphicsBackendType::VULKAN_API:
            glfwInit();
            m_window->init("Vector3D", rendering::WindowBackendHint::VULKAN_API);

            m_vulkanBackend = new rendering::VulkanBackend(m_window);
            m_graphicsBackend = m_vulkanBackend;
            // m_vulkanBackend->init();
            break;
        case rendering::GraphicsBackendType::OPENGL_API:
            glfwInit();
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
    }
    void Engine::start() {}

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
        // Initialize the scene and add entities
        m_scene = Scene::create();
        auto entity = m_scene->instantiateEntity("Test object");
        auto entity2 = m_scene->instantiateEntity("Test child object", entity);
        auto entity3 = m_scene->instantiateEntity("Test grandchild object", entity2);
        auto entity4 = m_scene->instantiateEntity("Test object 2");

        m_scene->instantiateEntityComponent<TestComponent>(entity.index());

        m_scene->print_entities();

        // Main game loop
        while (!m_window->shouldClose()) {
            const auto frame_start = std::chrono::steady_clock::now();

            // Poll for window events
            m_window->pollEvents();

            // Update logic
            m_scene->update(m_last_frame_dt.count());

            // Render frame
            m_graphicsBackend->frame_update();

            // Update deltatime
            const auto frame_end = std::chrono::steady_clock::now();
            const std::chrono::duration<double> diff = frame_end - frame_start;
            m_last_frame_dt = diff;
            std::cout << "Last frame dt (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "\n";
        }
    }
}
