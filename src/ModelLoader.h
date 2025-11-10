#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <plog/Severity.h>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "OBJ-Loader-master/Source/OBJ_Loader.h"

namespace v3d {
struct VertexLayout;

class ModelLoader {
   private:
   protected:
    std::string m_filePath;
    bool m_fileLoaded = false;

    ModelLoader() = default;
    ModelLoader(std::string_view filePath) : m_filePath(filePath) {};

    virtual void loadFile() = 0;

   public:
    ModelLoader(const ModelLoader&) = delete;
    ModelLoader& operator=(const ModelLoader&) = delete;
    virtual ~ModelLoader() = default;

    virtual void loadFile(std::string_view filePath) = 0;
    /// @brief Explicitly clear resources.
    virtual void clear() = 0;

    // Mesh data

    /// @brief Get num of loaded meshes
    virtual unsigned int getNumMeshes() = 0;

    /// @brief Get buffer containing the mesh vertex data
    /// @param meshIndex
    /// @return std::tuple<size_t, void*, VertexLayout> raw buffer size in
    /// memory (bytes), start of the buffer, vertex layout (info to interpret
    /// the data)
    virtual std::tuple<size_t, void*, VertexLayout> getMeshVertexBuffer(
        unsigned int meshIndex) = 0;
    /// @brief Get num of vertices
    /// @param meshIndex
    /// @return Num of vertices
    virtual size_t getMeshNumVertex(unsigned int meshIndex) = 0;

    /// @brief Get buffer containing the mesh vertex indices
    /// @param meshIndex
    /// @return pointer to the start of the buffer
    virtual unsigned int* getMeshIndicesBuffer(unsigned int meshIndex) = 0;
    /// @brief Get num of indices
    /// @param meshIndex
    /// @return Num of indices
    virtual size_t getMeshNumIndices(unsigned int meshIndex) = 0;

    /// @brief Get mesh display name
    virtual std::string_view getMeshName(unsigned int meshIndex) = 0;

    // Material Data

    virtual unsigned int getNumMaterials() = 0;
    virtual unsigned int getNumTextures() = 0;

    // Scene Data

    virtual unsigned int getNumLights() = 0;
    virtual unsigned int getNumCameras() = 0;

    // Misc Data
    virtual unsigned int getNumAnimations() = 0;
};

class OBJLoader : public ModelLoader {
   private:
    std::unique_ptr<objl::Loader> m_obj_loader = nullptr;

   protected:
    void loadFile() override;

   public:
    OBJLoader() = delete;
    OBJLoader(std::string_view filePath) : ModelLoader(filePath) {
        loadFile();
    };
    ~OBJLoader() override = default;

    void loadFile(std::string_view filePath) override;
    void clear() override;

    // TODO: Implement material and textures
    // Not implemented
    unsigned int getNumMaterials() override { return 0; };
    unsigned int getNumTextures() override { return 0; };

    // Mesh data

    unsigned int getNumMeshes() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        return 1;
    };

    std::tuple<size_t, void*, VertexLayout> getMeshVertexBuffer(
        unsigned int meshIndex) override;
    size_t getMeshNumVertex(unsigned int meshIndex) override;

    unsigned int* getMeshIndicesBuffer(unsigned int meshIndex) override;
    size_t getMeshNumIndices(unsigned int meshIndex) override;

    std::string_view getMeshName(unsigned int meshIndex) override;

    // Not supported
    unsigned int getNumAnimations() override { return 0; };
    unsigned int getNumCameras() override { return 0; };
    unsigned int getNumLights() override { return 0; };
};

struct AssimpLoaderPropertes {
    // by default triangulate
    aiPostProcessSteps postProcessingFlags =
        aiPostProcessSteps::aiProcess_Triangulate;

    std::vector<std::pair<std::string, int>> integerProperty;
    std::vector<std::pair<std::string, bool>> boolProperty;
    std::vector<std::pair<std::string, ai_real>> floatProperty;
    std::vector<std::pair<std::string, std::string>> stringProperty;
    std::vector<std::pair<std::string, aiMatrix4x4>> matrixProperty;
    std::vector<std::pair<std::string, void*>> pointerProperty;

    void SetPropertyInteger(const char* szName, int iValue) {
        integerProperty.push_back(std::make_pair(szName, iValue));
    }
    void SetPropertyBool(const char* szName, bool value) {
        boolProperty.push_back(std::make_pair(szName, value));
    }
    void SetPropertyFloat(const char* szName, ai_real fValue) {
        floatProperty.push_back(std::make_pair(szName, fValue));
    }
    void SetPropertyString(const char* szName, const std::string& sValue) {
        stringProperty.push_back(std::make_pair(szName, sValue));
    }
    void SetPropertyMatrix(const char* szName, const aiMatrix4x4& sValue) {
        matrixProperty.push_back(std::make_pair(szName, sValue));
    }
    void SetPropertyPointer(const char* szName, void* sValue) {
        pointerProperty.push_back(std::make_pair(szName, sValue));
    }
};

struct AssimpLoaderVertex {
    aiVector3D vertice;
    aiVector3D normal;
    aiVector2D textureCoord;
    aiVector3D tangent;
    aiVector3D bitangent;
    aiColor4D color;
};

class AssimpLogStream : public Assimp::LogStream {
   private:
    plog::Severity m_pSeverity;

   public:
    AssimpLogStream() : m_pSeverity(plog::Severity::verbose) {};
    AssimpLogStream(plog::Severity severity) : m_pSeverity(severity) {};

    // Write something using your own functionality
    void write(const char* message);
};

class AssimpLoader : public ModelLoader {
   private:
    Assimp::Importer m_importer;
    const AssimpLoaderPropertes m_importerProperties;

    const aiScene* m_scene = nullptr;

    std::vector<std::vector<AssimpLoaderVertex>> m_meshVertexData;
    std::vector<std::vector<unsigned int>> m_meshIndices;

   protected:
    void loadFile() override;

   public:
    AssimpLoader() = delete;
    AssimpLoader(AssimpLoaderPropertes&& properties)
        : m_importerProperties(properties) {};
    AssimpLoader(std::string_view filePath, AssimpLoaderPropertes&& properties)
        : ModelLoader(filePath), m_importerProperties(properties) {
        loadFile();
    }
    ~AssimpLoader() override = default;

    static void attachDefaultLogger() {
        // Attaching handler to the default logger
        Assimp::DefaultLogger::get()->attachStream(
            new AssimpLogStream(plog::Severity::error), Assimp::Logger::Err);
        Assimp::DefaultLogger::get()->attachStream(
            new AssimpLogStream(plog::Severity::info), Assimp::Logger::Warn);
        Assimp::DefaultLogger::get()->attachStream(
            new AssimpLogStream(plog::Severity::debug), Assimp::Logger::Info);
        Assimp::DefaultLogger::get()->attachStream(
            new AssimpLogStream(plog::Severity::verbose),
            Assimp::Logger::Debugging);
    }

    void loadFile(std::string_view filePath) override;
    void clear() override { m_importer.FreeScene(); };

    unsigned int getNumMeshes() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        assert(m_scene && "Invalid scene state");
        return m_scene->mNumMeshes;
    };
    unsigned int getNumMaterials() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        assert(m_scene && "Invalid scene state");
        return m_scene->mNumMaterials;
    };
    unsigned int getNumAnimations() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        assert(m_scene && "Invalid scene state");
        return m_scene->mNumAnimations;
    };
    unsigned int getNumTextures() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        assert(m_scene && "Invalid scene state");
        return m_scene->mNumTextures;
    };
    unsigned int getNumLights() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        assert(m_scene && "Invalid scene state");
        return m_scene->mNumLights;
    };
    unsigned int getNumCameras() override {
        assert(m_fileLoaded &&
               "Tried to fetch meshes count before loading file");
        assert(m_scene && "Invalid scene state");
        return m_scene->mNumCameras;
    };

    std::tuple<size_t, void*, VertexLayout> getMeshVertexBuffer(
        unsigned int meshIndex) override;
    size_t getMeshNumVertex(unsigned int meshIndex) override;

    unsigned int* getMeshIndicesBuffer(unsigned int meshIndex) override;
    size_t getMeshNumIndices(unsigned int meshIndex) override;
    std::string_view getMeshName(unsigned int meshIndex) override;
};

}  // namespace v3d
