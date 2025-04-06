#pragma once

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <glad/glad.h>

#include "rendering/vulkan_backend.h"
#include "rendering/opengl_backend.h"
#include "window.h"

namespace v3d {

namespace rendering {
enum class GraphicsBackendType { OPENGL_API, VULKAN_API };
}  // namespace rendering

class Engine {
   public:
    Engine() : m_window(new Window()) {};
    Engine(uint32_t width, uint32_t height) : m_window(new Window(width, height)) {};
    Engine(uint32_t width, uint32_t height, rendering::GraphicsBackendType graphicsBackendType) : m_window(new Window(width, height)), m_gBackendType(graphicsBackendType) {};

    void run() {
        init();
        start();
        mainLoop();
        cleanup();
    }

   private:
    rendering::GraphicsBackendType m_gBackendType = rendering::GraphicsBackendType::VULKAN_API;
    rendering::GraphicsBackend *m_graphicsBackend;
    rendering::VulkanBackend *m_vulkanBackend;
    rendering::OpenGlBackend *m_openGlBackend;
    Window *m_window;

    bool m_initialized = false;

    void init();
    void start();
    void mainLoop();
    void cleanup();
};
}  // namespace v3d
