#include "engine.h"
#include <plog/Log.h>

namespace v3d {
    void Engine::init() {
        if (m_initialized) {
            throw std::runtime_error("Engine initialized multiple times");
        }

        PLOGI << "Initializing Engine" << std::endl;

        // Initialize GLFW and create a window
        glfwInit();
        switch (m_gBackendType)
        {
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
        while (!m_window->shouldClose()) {
            m_graphicsBackend->frame_update();
            m_window->pollEvents();
        }
    }
}
