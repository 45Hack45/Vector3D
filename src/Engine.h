#pragma once

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "rendering/vulkan_backend.h"
#include "window.h"

namespace v3d {

class Engine {
   public:
    Engine() : m_window(new Window()) {};
    Engine(uint32_t width, uint32_t height) : m_window(new Window(width, height)) {};

    void run() {
        init();
        start();
        mainLoop();
        cleanup();
    }

   private:
    rendering::VulkanBackend *m_vulkanBackend;
    Window *m_window;

    bool m_initialized = false;

    void init();
    void start();
    void mainLoop();
    void cleanup();
};
}  // namespace v3d
