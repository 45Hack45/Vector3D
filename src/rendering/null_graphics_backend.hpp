#pragma once

#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {


class NullGraphicsBackend : public GraphicsBackend {
   public:
   NullGraphicsBackend(Window* window) : GraphicsBackend(window) {}

    void init(){m_initialized = true;};

    void frame_update(){
        return;
    };

    void cleanup(){m_initialized = false;};

   private:


};
}  // namespace rendering
}  // namespace v3d