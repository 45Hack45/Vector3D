#pragma once

#include <GLFW/glfw3.h>

namespace v3d {
class GraphicsBackend {
   public:
    virtual void init() = 0;
    virtual void frame_update() {};
    virtual void destroy() = 0;

    GraphicsBackend(GLFWwindow *window) : window(window) {}

   protected:
    GLFWwindow *window;

   private:
};
}  // namespace v3d