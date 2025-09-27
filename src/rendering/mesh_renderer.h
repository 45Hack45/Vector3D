#pragma once
#include "component.h"
#include "rendering/rendering_def.h"

namespace v3d {
class Mesh;
class Transform;

class MeshRenderer : public rendering::IRenderable, public ComponentBase {
   public:
    MeshRenderer();
    ~MeshRenderer() override;

    // TODO: Missing dependancy with Transform
    // static auto dependencies();

    std::string getComponentName() override { return "MeshRenderer"; };

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    void setMesh(Mesh* mesh) {
        m_mesh = mesh;
        registerRenderTarget();
    };
    void resetMesh() {
        m_mesh = nullptr;
        // TODO: Unregister render target
    };

   private:
    Transform* m_transform = nullptr;
    Mesh* m_mesh = nullptr;

    void renderElement() override;
    void renderElementInstanced() override;

    void setUniforms(Shader* shader) override;

    void registerRenderTarget();
    // TODO: Implement unregisterRenderTarget
};
}  // namespace v3d
