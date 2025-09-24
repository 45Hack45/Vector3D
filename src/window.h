#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "rendering/rendering_def.h"

namespace v3d {

class Window {
   public:
    Window() : m_width(800), m_height(600) {}
    Window(uint32_t width, uint32_t height)
        : m_width(width), m_height(height) {}
    ~Window() { cleanup(); };

    void init(const char *title, rendering::WindowBackendHint api);

    void cleanup();

    GLFWwindow *getWindow() { return m_window; }

    // void setSize(uint32_t width, uint32_t height) {
    //     m_width = width;
    //     m_height = height;
    // }
    uint32_t getWidth() { return m_width; }
    uint32_t getHeight() { return m_height; }

    bool shouldClose() {
        if (m_window)
            return glfwWindowShouldClose(m_window);
        else
            return false;
    }
    void pollEvents() { glfwPollEvents(); }

   private:
    bool m_initialized = false;
    GLFWwindow *m_window;
    uint32_t m_width, m_height;
};

}  // namespace v3d
