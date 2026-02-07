#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "rendering/rendering_def.h"

namespace v3d {

class Window {
   public:
    /// @brief Create instance without an actual window associated. Useful for
    /// running headless.
    Window() : m_width(800), m_height(600) {}
    /**
     * @brief Create and initialize a window with the given title and rendering
     * API.
     *
     * @param title The title of the window.
     * @param api The rendering API to use for the window. If set to
     *            WindowBackendHint::VULKAN_API. Note the window will not be
     *            resizable.
     * @param width Window initial width
     * @param height Window initial height
     * @param windowScale Scale the window size preserving the aspect ratio
     * @param swapInterval 
     */
    Window(const char* title, rendering::WindowBackendHint api, uint32_t width,
           uint32_t height, float windowScale = 1.f, bool enableVsync = true);
    ~Window();

    GLFWwindow* getWindow() { return m_window; }

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
    bool m_glfwWindowInitialized = false;
    GLFWwindow* m_window;
    uint32_t m_width = 800, m_height = 600;
};

}  // namespace v3d
