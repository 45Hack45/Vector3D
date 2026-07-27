
#include "window.h"

#include <plog/Log.h>

#include <cassert>

namespace v3d {

Window::Window(const char* title, rendering::WindowBackendHint api,
               uint32_t width, uint32_t height, float windowScale,
               bool enableVsync) {
    PLOGI << "Initializing window with size " << width << "x" << height
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

    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
    m_width = static_cast<uint32_t>(fbWidth);
    m_height = static_cast<uint32_t>(fbHeight);

    // Window owns the user pointer, subsystems register listeners
    // rather than installing GLFW callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, handleFramebufferSize);
    glfwSetCursorPosCallback(m_window, handleCursorPos);
    glfwSetScrollCallback(m_window, handleScroll);
    glfwSetMouseButtonCallback(m_window, handleMouseButton);

    glfwSwapInterval(enableVsync ? 1 : 0);  // Enable/Disable vsync

    m_glfwWindowInitialized = true;
}

Window::~Window() {
    if (m_glfwWindowInitialized) {
        glfwDestroyWindow(m_window);
    }
}

void Window::handleFramebufferSize(GLFWwindow* handle, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if (!self) return;
    dispatch(self->m_registry->framebufferSize, width, height);
}

void Window::handleCursorPos(GLFWwindow* handle, double xpos, double ypos) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if (!self) return;
    dispatch(self->m_registry->cursorPos, xpos, ypos);
}

void Window::handleScroll(GLFWwindow* handle, double xoffset, double yoffset) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if (!self) return;
    dispatch(self->m_registry->scroll, xoffset, yoffset);
}

void Window::handleMouseButton(GLFWwindow* handle, int button, int action,
                               int mods) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if (!self) return;
    dispatch(self->m_registry->mouseButton, button, action, mods);
}

}  // namespace v3d
