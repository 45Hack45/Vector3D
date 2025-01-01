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
        m_window->init("Vector3D", rendering::WindowBackendHint::VULKAN_API);

        m_vulkanBackend = new rendering::VulkanBackend(m_window);
        m_vulkanBackend->init();

        m_initialized = true;
    }
    void Engine::start() {}

    void Engine::cleanup() {
        if (!m_initialized) {
            return;
        }
        m_vulkanBackend->cleanup();
        m_window->cleanup();
        glfwTerminate();

        m_initialized = false;
    }

    void Engine::mainLoop() {
        while (!m_window->shouldClose()) {
            m_vulkanBackend->frame_update();
            m_window->pollEvents();
        }
    }
}
