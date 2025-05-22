#pragma once

#include "mesh.h"
#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {

class OpenGlBackend : public GraphicsBackend {
   public:
    OpenGlBackend(Window* window) : GraphicsBackend(window) {}

    void init() override;

    void frame_update() override;

    void cleanup() override;

    Mesh* createMesh(std::string filePath) override;

   protected:
    void initPrimitives() override;

   private:
    std::vector<MeshOpenGL*> m_meshList;
};
}  // namespace rendering
}  // namespace v3d