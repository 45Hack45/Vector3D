#pragma once

#include <string>

#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {

class NullGraphicsBackend : public GraphicsBackend {
   public:
    NullGraphicsBackend(Window* window) : GraphicsBackend(window) {}

    void init() { m_initialized = true; };

    void frame_update() {
        return;
    };

    void cleanup() { m_initialized = false; };

    Mesh* createMesh(std::string filePath) override {
        // TODO: Implement MeshNull, load the data but do nothing
        return nullptr;
    };

   private:
};
}  // namespace rendering
}  // namespace v3d