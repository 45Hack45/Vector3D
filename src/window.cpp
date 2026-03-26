
#include "window.h"

#include <plog/Log.h>

#include <cassert>

namespace v3d {

Window::Window(const char* title, rendering::WindowBackendHint api,
               uint32_t width, uint32_t height, float windowScale,
               bool enableVsync) {
    PLOGI << "Initializing window with size " << m_width << "x" << m_height
          << " and title \"" << title << "\"" << std::endl;

    switch (api) {
        case v3d::rendering::WindowBackendHint::NONE:
            PLOGI << "  - Windowless mode";
            m_window = NULL;
            m_glfwWindowInitialized = true;
            return;
        case rendering::WindowBackendHint::VULKAN_API:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            // Window resizing is not implemented for Vulkan
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            break;
        case rendering::WindowBackendHint::OPENGL_API:
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            // glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            break;
        default:
            break;
    }

    assert(windowScale >= 1 && "Invalid window scale");
    assert(width > 0 && height > 0 && "Invalid window resolution");

    m_width = width;
    m_height = height;

    m_window = glfwCreateWindow((int)(m_width * windowScale),
                                (int)(m_height * windowScale), title, nullptr,
                                nullptr);

    if (m_window == NULL) {
        PLOG_ERROR << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    glfwSwapInterval(enableVsync ? 1 : 0);  // Enable/Disable vsync

    m_glfwWindowInitialized = true;
}

Window::~Window() {
    if (m_glfwWindowInitialized) {
        glfwDestroyWindow(m_window);
    }
}

}  // namespace v3d
