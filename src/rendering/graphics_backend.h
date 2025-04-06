#pragma once

#include "window.h"

namespace v3d {
namespace rendering {
class GraphicsBackend {
   public:
    virtual void init() = 0;
    virtual void frame_update() {};
    virtual void cleanup() = 0;

    GraphicsBackend(Window *window) : m_window(window) {}

   protected:
    Window *m_window;

   private:
};
}  // namespace rendering
}  // namespace v3d