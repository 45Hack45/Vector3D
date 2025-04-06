
#include "window.h"

#include <plog/Log.h>

namespace v3d {
Window::~Window() {
    if (m_initialized) {
        glfwDestroyWindow(m_window);
    }
}


/**
 * @brief Initializes the window with the given title and rendering API.
 *
 * @param title The title of the window.
 * @param api The rendering API to use for the window. If set to
 *            WindowBackendHint::VULKAN_API, the window will not be
 *            resizable.
 */
void Window::init(const char *title, rendering::WindowBackendHint api) {

    PLOGI << "Initializing window with size " << m_width << "x" << m_height
          << " and title \"" << title << "\"" << std::endl;

    if (m_initialized) {
        PLOG_WARNING << "Tried to initialize window that was already "
                        "initialized"
                     << std::endl;
        return;
    }

    switch (api)
    {
    case rendering::WindowBackendHint::VULKAN_API:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Window resizing is not implemented for Vulkan
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        break;
    case rendering::WindowBackendHint::OPENGL_API:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        break;
    default:
        break;
    }
    
    m_window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
    m_initialized = true;
}

void Window::cleanup() {
    if (m_initialized) {
        glfwDestroyWindow(m_window);
    } else {
        PLOG_WARNING << "Tried to cleanup window that was not initialized"
                     << std::endl;
    }
}
}  // namespace v3d
