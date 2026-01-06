#pragma once

#include <memory>
#include <vector>

#include "Model.h"
#include "ModelLoader.h"

namespace v3d {
class ModelManager {
   private:
    std::unique_ptr<ModelLoader> m_loader;
    std::vector<std::unique_ptr<Model>> m_models;
    std::vector<std::unique_ptr<Mesh>> m_meshes;

   public:
    explicit ModelManager(std::unique_ptr<ModelLoader> loader)
        : m_loader(std::move(loader)) {
        if (!m_loader)
            throw std::invalid_argument(
                "ModelManager requires a valid ModelLoader instance");
    };
    ~ModelManager() = default;

    // Compile-time non nullptr enforcement
    ModelManager(std::nullptr_t) = delete;

    // copy semantics
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    // move semantics
    ModelManager(ModelManager&&) noexcept = default;
    ModelManager& operator=(ModelManager&&) noexcept = default;

    template <typename... Args>
    static std::unique_ptr<ModelManager> make_unique(Args&&... args) {
        return std::make_unique<ModelManager>(std::forward<Args>(args)...);
    }

    template <typename MeshType>
    Model* importModel(const std::string& filepath, const std::string& name) {
        static_assert(std::is_base_of<Mesh, MeshType>::value,
                      "MeshType must inherit from the base class Mesh");

        m_loader->loadFile(filepath);
        std::unique_ptr<Model> model = Model::make_unique(filepath, name);
        Model* model_ptr = model.get();

        for (int i = 0; i < m_loader->getNumMeshes(); i++) {
            auto [vertexBufferSize, vertexBuffer, vertexLayout] =
                m_loader->getMeshVertexBuffer(i);

            unsigned int* indicesBuffer = m_loader->getMeshIndicesBuffer(i);
            size_t numIndices = m_loader->getMeshNumIndices(i);
            std::unique_ptr<Mesh> mesh = std::make_unique<MeshType>(
                vertexBuffer, vertexBufferSize, vertexLayout, indicesBuffer,
                numIndices);

            mesh->m_name = m_loader->getMeshName(i);

            model->m_meshes.push_back(mesh.get());
            m_meshes.push_back(std::move(mesh));
        }
        m_models.push_back(std::move(model));
        return model_ptr;
    }
};

}  // namespace v3d
