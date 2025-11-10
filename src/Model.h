#pragma once

#include "Mesh.h"
#include "asset.h"

namespace v3d {
class ModelManager;
class Model : public Asset {
    friend class ModelManager;

   private:
    std::vector<const Mesh*> m_meshes;

    Model() = delete;
    Model(const std::string& file, const std::string& name)
        : Asset(file, name) {}

    template <typename... Args>
    static std::unique_ptr<Model> make_unique(Args&&... args) {
        return std::unique_ptr<Model>(new Model(std::forward<Args>(args)...));
    }

   public:
    ~Model() override = default;

    std::vector<const Mesh*> getMeshes() const { return m_meshes; }
};

}  // namespace v3d
