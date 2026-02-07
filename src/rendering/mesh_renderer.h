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

    void setMesh(const Mesh* mesh) {
        m_mesh = mesh;
        registerRenderTarget();
    };
    void resetMesh() {
        unregisterRenderTarget();
        m_mesh = nullptr;
    };

   private:
    Transform* m_transform = nullptr;
    const Mesh* m_mesh = nullptr;

    void renderElement() override;
    void renderElementInstanced() override;

    void setUniforms(Shader* shader) override;

    void registerRenderTarget();
    void unregisterRenderTarget();
};
}  // namespace v3d
