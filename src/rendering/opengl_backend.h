#pragma once

#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {


class OpenGlBackend : public GraphicsBackend {
   public:
    OpenGlBackend(Window* window) : GraphicsBackend(window) {}

    void init();

    void frame_update();

    void cleanup();

   private:
    bool m_initialized = false;


};
}  // namespace rendering
}  // namespace v3d