#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "rendering/rendering_def.h"

namespace v3d {

/// @brief Owns the GLFW window and dispatches its events to any number of
/// listeners.
///
/// The Window holds the GLFW user pointer and the callback slot for every
/// event it dispatches. Register with onFramebufferSize(), onScroll(),
/// onCursorPos() or onMouseButton() and hold the returned Subscription.
/// Anything that chains to a previous GLFW callback, such as Dear ImGui, must
/// be initialized after the Window.
///
/// A listener must not subscribe or unsubscribe from inside a callback.
class Window {
   public:
    using FramebufferSizeCallback = std::function<void(int width, int height)>;
    using CursorPosCallback = std::function<void(double xpos, double ypos)>;
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;
    using MouseButtonCallback =
        std::function<void(int button, int action, int mods)>;

   private:
    enum class Event { FramebufferSize, CursorPos, Scroll, MouseButton };

    template <typename TCallback>
    struct Listener {
        uint32_t token;
        TCallback fn;
    };

    // Listener lists. A Subscription may outlive its Window.
    struct Registry {
        std::vector<Listener<FramebufferSizeCallback>> framebufferSize;
        std::vector<Listener<CursorPosCallback>> cursorPos;
        std::vector<Listener<ScrollCallback>> scroll;
        std::vector<Listener<MouseButtonCallback>> mouseButton;
        uint32_t nextToken = 1;
    };

   public:
    /// @brief Handle to one registered listener. Unsubscribes when destroyed.
    /// Move-only: assigning over a live Subscription drops the old listener.
    class Subscription {
       public:
        Subscription() = default;
        ~Subscription() { reset(); }

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        Subscription(Subscription&& other) noexcept { moveFrom(other); }
        Subscription& operator=(Subscription&& other) noexcept {
            if (this != &other) {
                reset();
                moveFrom(other);
            }
            return *this;
        }

        /// @brief Unsubscribe now rather than at end of scope.
        void reset();

        /// @brief Whether this handle still refers to a live registration.
        bool isActive() const { return !m_registry.expired(); }

       private:
        friend class Window;
        Subscription(std::weak_ptr<Registry> registry, Event event,
                     uint32_t token)
            : m_registry(std::move(registry)),
              m_event(event),
              m_token(token) {}

        void moveFrom(Subscription& other) {
            m_registry = std::move(other.m_registry);
            m_event = other.m_event;
            m_token = other.m_token;
            other.m_registry.reset();
            other.m_token = 0;
        }

        std::weak_ptr<Registry> m_registry;
        Event m_event = Event::FramebufferSize;
        uint32_t m_token = 0;
    };

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

    /// @brief Update the saved framebuffer size.
    void setFramebufferSize(uint32_t width, uint32_t height) {
        m_width = width;
        m_height = height;
    }
    uint32_t getWidth() { return m_width; }
    uint32_t getHeight() { return m_height; }

    /// @brief Framebuffer aspect ratio. Returns 1 while the window is
    ///        minimized (height 0).
    float getAspectRatio() {
        if (m_height == 0) return 1.f;
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }

    bool shouldClose() {
        if (m_window)
            return glfwWindowShouldClose(m_window);
        else
            return false;
    }
    void pollEvents() { glfwPollEvents(); }

    /// @brief Listen for framebuffer resizes. Sizes are in pixels.
    /// @return Handle whose destruction unsubscribes.
    Subscription onFramebufferSize(FramebufferSizeCallback cb) {
        return add(m_registry->framebufferSize, Event::FramebufferSize,
                   std::move(cb));
    }
    Subscription onCursorPos(CursorPosCallback cb) {
        return add(m_registry->cursorPos, Event::CursorPos, std::move(cb));
    }
    Subscription onScroll(ScrollCallback cb) {
        return add(m_registry->scroll, Event::Scroll, std::move(cb));
    }
    Subscription onMouseButton(MouseButtonCallback cb) {
        return add(m_registry->mouseButton, Event::MouseButton, std::move(cb));
    }

   private:
    // GLFW entry points. Recover the Window from the user pointer and
    // dispatch.
    static void handleFramebufferSize(GLFWwindow* handle, int width,
                                      int height);
    static void handleCursorPos(GLFWwindow* handle, double xpos, double ypos);
    static void handleScroll(GLFWwindow* handle, double xoffset,
                             double yoffset);
    static void handleMouseButton(GLFWwindow* handle, int button, int action,
                                  int mods);

    template <typename TCallback>
    Subscription add(std::vector<Listener<TCallback>>& listeners, Event event,
                     TCallback cb) {
        const uint32_t token = m_registry->nextToken++;
        listeners.push_back({token, std::move(cb)});
        return Subscription(m_registry, event, token);
    }

    template <typename TCallback, typename... TArgs>
    static void dispatch(std::vector<Listener<TCallback>>& listeners,
                         TArgs... args) {
        for (std::size_t i = 0; i < listeners.size(); i++)
            listeners[i].fn(args...);
    }

    bool m_glfwWindowInitialized = false;
    GLFWwindow* m_window;
    // Framebuffer size in pixels, not window size.
    uint32_t m_width = 800, m_height = 600;

    std::shared_ptr<Registry> m_registry = std::make_shared<Registry>();
};

inline void Window::Subscription::reset() {
    if (auto registry = m_registry.lock()) {
        auto erase = [this](auto& listeners) {
            for (std::size_t i = 0; i < listeners.size(); i++) {
                if (listeners[i].token == m_token) {
                    listeners.erase(listeners.begin() + i);
                    return;
                }
            }
        };

        switch (m_event) {
            case Event::FramebufferSize:
                erase(registry->framebufferSize);
                break;
            case Event::CursorPos:
                erase(registry->cursorPos);
                break;
            case Event::Scroll:
                erase(registry->scroll);
                break;
            case Event::MouseButton:
                erase(registry->mouseButton);
                break;
        }
    }
    m_registry.reset();
    m_token = 0;
}

}  // namespace v3d
