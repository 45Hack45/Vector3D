#pragma once

#include <glad/glad.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "input/InputDevice.hpp"
#include "input/InputKeys.hpp"
#include "input/InputManager.h"
#include "physics/physics.h"
#include "plog/Log.h"
#include "rendering/null_graphics_backend.hpp"
#include "rendering/opengl_backend.h"
#include "rendering/rendering_def.h"
#include "rendering/vulkan_backend.h"
#include "scene.h"
#include "window.h"

namespace v3d {

class Engine {
   public:
    Engine() : m_window(new Window()) {};
    Engine(uint32_t width, uint32_t height)
        : m_window(new Window(width, height)) {};
    Engine(uint32_t width, uint32_t height,
           rendering::GraphicsBackendType graphicsBackendType)
        : m_window(new Window(width, height)),
          m_gBackendType(graphicsBackendType) {};

    void run() {
        m_engineStartTime = std::chrono::steady_clock::now();
        init();
        start();
        mainLoop();
        cleanup();
    }

    inline void registerRenderTarget(rendering::IRenderable *renderTarget) {
        m_graphicsBackend->registerRenderTarget(renderTarget);
    }

    inline void registerGizmosTarget(
        rendering::IGizmosRenderable *gizmosTarget) {
        m_graphicsBackend->registerGizmosTarget(gizmosTarget);
    }

    InputManager *getInputManager() { return &m_inputManager; }

   private:
    rendering::GraphicsBackendType m_gBackendType =
        rendering::GraphicsBackendType::VULKAN_API;
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

    void processInput(GLFWwindow *window);

    GLFWwindow *window;

    InputManager m_inputManager;

    void initDefaultInput();

    Physics m_phSystem;
    std::shared_ptr<Scene> m_scene;
    std::chrono::steady_clock::time_point m_engineStartTime;
    std::chrono::duration<double> m_last_frame_dt =
        std::chrono::duration<double>(1 / 60);
};
}  // namespace v3d
