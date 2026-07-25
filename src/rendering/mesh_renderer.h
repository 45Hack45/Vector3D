#pragma once
#include "component.h"
#include "rendering/rendering_def.h"

namespace v3d {
class Mesh;
class Transform;

class MeshRenderer : public rendering::IRenderable, public ComponentBase {
    friend class boost::serialization::access;

   public:
    MeshRenderer();
    ~MeshRenderer() override;

    // TODO: Missing dependancy with Transform
    // static auto dependencies();

    std::string getComponentName() override { return "MeshRenderer"; };
    static std::string getName() { return "MeshRenderer"; };

    void init() override;
    void start() override {};
    void update(double deltaTime) override {};

    void setMesh(const Mesh* mesh) {
        m_mesh = mesh;
        registerRenderTarget();
    };

    // Store the model file path and mesh name alongside the pointer so
    // the renderer can be identified and reloaded after deserialization.
    void setMesh(const Mesh* mesh, std::string modelPath, std::string meshName) {
        m_meshModelPath = std::move(modelPath);
        m_meshName = std::move(meshName);
        setMesh(mesh);
    };

    void resetMesh() {
        unregisterRenderTarget();
        m_mesh = nullptr;
    };

   private:
    Transform* m_transform = nullptr;
    const Mesh* m_mesh = nullptr;

    // Stable mesh identity used for serialization; restored by init() once
    // ModelManager is available.
    // TODO: reload mesh from ModelManager in init() when m_meshModelPath is set
    std::string m_meshModelPath;
    std::string m_meshName;

    void renderElement() override;
    void renderElementInstanced() override;

    void setUniforms(Shader* shader) override;

    void registerRenderTarget();
    void unregisterRenderTarget();

    template <class Archive>
    void save(Archive& ar, unsigned int /*version*/) const {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        ar& BOOST_SERIALIZATION_NVP(m_meshModelPath);
        ar& BOOST_SERIALIZATION_NVP(m_meshName);
        // m_mesh and m_transform are runtime pointers, not serialized.
    }

    template <class Archive>
    void load(Archive& ar, unsigned int /*version*/) {
        ar& boost::serialization::make_nvp("ComponentBase",
                                           boost::serialization::base_object<ComponentBase>(*this));
        ar& BOOST_SERIALIZATION_NVP(m_meshModelPath);
        ar& BOOST_SERIALIZATION_NVP(m_meshName);
        // m_mesh is NOT automatically reloaded here; see TODO above.
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
}  // namespace v3d
