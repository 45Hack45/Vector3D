#pragma once

#include <string>

#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {

class NullGraphicsBackend : public GraphicsBackend {
   public:
    NullGraphicsBackend(Window* window) : GraphicsBackend(window) {}

    Mesh* createMesh(std::string filePath) override {
        // TODO: Implement MeshNull, load the data but do nothing
        return nullptr;
    };

   protected:
    void initPrimitives() override {};
    void frameUpdate() override {};
    void presentFrame() override {};

   private:
};
}  // namespace rendering
}  // namespace v3d