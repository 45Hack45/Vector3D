#include "Engine.h"

namespace v3d {
    void Engine::init() {
        if (initialized) {
            throw std::runtime_error("Engine initialized multiple times");
        }
        initWindow();
        vulkanBackend = new VulkanBackend(window);
        vulkanBackend->init();

        initialized = true;
    }
    void Engine::start() {}

    void Engine::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(m_width, m_height, "Vector3D", nullptr, nullptr);
    }

    void Engine::cleanup() {
        if (!initialized) {
            return;
        }
        vulkanBackend->destroy();
        glfwDestroyWindow(window);
        glfwTerminate();

        initialized = false;
    }

    void Engine::mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            vulkanBackend->frame_update();
            glfwPollEvents();
        }
    }
}
