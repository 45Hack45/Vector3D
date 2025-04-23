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
    virtual ~GraphicsBackend(){}

   protected:
    Window *m_window;
    bool m_initialized = false;

   private:
};
}  // namespace rendering
}  // namespace v3d