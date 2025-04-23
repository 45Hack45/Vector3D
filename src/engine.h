#pragma once

#include <plog/Log.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <glad/glad.h>

#include "rendering/vulkan_backend.h"
#include "rendering/opengl_backend.h"
#include "rendering/null_graphics_backend.hpp"
#include "window.h"

#include "scene.h"

namespace v3d {

namespace rendering {
enum class GraphicsBackendType { NONE, OPENGL_API, VULKAN_API };
}  // namespace rendering

class Engine {
   public:
    Engine() : m_window(new Window()) {};
    Engine(uint32_t width, uint32_t height) : m_window(new Window(width, height)) {};
    Engine(uint32_t width, uint32_t height, rendering::GraphicsBackendType graphicsBackendType) : m_window(new Window(width, height)), m_gBackendType(graphicsBackendType) {};

    void run() {
        m_engineStartTime = std::chrono::steady_clock::now();
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
    rendering::NullGraphicsBackend *m_nullGraphicsBackend;
    Window *m_window;
    
    bool m_initialized = false;

    void init();
    void start();
    void mainLoop();
    void cleanup();

    GLFWwindow *window;

    std::shared_ptr<Scene> m_scene;
    std::chrono::steady_clock::time_point m_engineStartTime;
    std::chrono::duration<double> m_last_frame_dt;
};
}  // namespace v3d
