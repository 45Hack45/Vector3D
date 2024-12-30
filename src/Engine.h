#pragma once

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "graphics_backend/VulkanBackend.h"

namespace v3d {


class Engine {
   public:
    Engine() : m_width(800), m_height(600) {}
    Engine(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}

    void run() {
        init();
        start();
        mainLoop();
        cleanup();
    }

   private:
    GLFWwindow *window;
    VulkanBackend *vulkanBackend;
    uint32_t m_width, m_height;

    bool initialized = false;

    void init();
    void initWindow();
    void start();
    void mainLoop();
    void cleanup();
};
}  // namespace v3d
