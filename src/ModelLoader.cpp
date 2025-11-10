#include "ModelLoader.h"

#include <glad/glad.h>
#include <plog/Log.h>

#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "Mesh.h"

constexpr VkFormat MapGLTypeToVkFormat(GLenum glType, int components,
                                       bool normalized = false) {
    if (glType == GL_FLOAT) {
        switch (components) {
            case 1:
                return VK_FORMAT_R32_SFLOAT;
            case 2:
                return VK_FORMAT_R32G32_SFLOAT;
            case 3:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case 4:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
    }
    if (glType == GL_UNSIGNED_BYTE) {
        switch (components) {
            case 1:
                return normalized ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_UINT;
            case 2:
                return normalized ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_UINT;
            case 3:
                return normalized ? VK_FORMAT_R8G8B8_UNORM
                                  : VK_FORMAT_R8G8B8_UINT;
            case 4:
                return normalized ? VK_FORMAT_R8G8B8A8_UNORM
                                  : VK_FORMAT_R8G8B8A8_UINT;
        }
    }
    throw std::runtime_error("Unsupported GL type/size -> VkFormat mapping.");
}

namespace v3d {
void OBJLoader::loadFile() {
    if (m_obj_loader == nullptr)
        m_obj_loader = std::make_unique<objl::Loader>();

    // Load .obj File
    PLOGD << "Loading model " << m_filePath << "...\n";
    bool loadout = m_obj_loader->LoadFile(m_filePath);

    if (!loadout) {
        m_fileLoaded = false;
        std::runtime_error("OBJLoader file \"" + m_filePath +
                           "\" not found or format not supported");
    }
    m_fileLoaded = true;
}

void OBJLoader::loadFile(std::string_view filePath) {
    m_filePath = filePath;
    loadFile();
}

void OBJLoader::clear() {
    // Delete the instance to cleare its internal resources
    m_obj_loader.reset();
}

// TODO: OBJLoader can load multiple meshes form the same obj file, implement
// multi mesh
std::tuple<size_t, void*, VertexLayout> OBJLoader::getMeshVertexBuffer(
    unsigned int meshIndex) {
    VertexLayout vLayout;

    vLayout.stride = sizeof(objl::Vertex);

    vLayout.attributes.push_back(
        VertexAttribute{0, 3, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(objl::Vertex, Position)});
    vLayout.attributes.push_back(
        VertexAttribute{1, 3, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(objl::Vertex, Normal)});
    vLayout.attributes.push_back(
        VertexAttribute{2, 2, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(objl::Vertex, TextureCoordinate)});

    return std::tuple<size_t, void*, VertexLayout>(
        m_obj_loader->LoadedVertices.size() * sizeof(objl::Vertex),
        m_obj_loader->LoadedVertices.data(), vLayout);
}

size_t OBJLoader::getMeshNumVertex(unsigned int meshIndex) {
    return m_obj_loader->LoadedVertices.size();
}

unsigned int* OBJLoader::getMeshIndicesBuffer(unsigned int meshIndex) {
    return m_obj_loader->LoadedIndices.data();
}

size_t OBJLoader::getMeshNumIndices(unsigned int meshIndex) {
    return m_obj_loader->LoadedIndices.size();
}

std::string_view OBJLoader::getMeshName(unsigned int meshIndex) {
    return m_obj_loader->LoadedMeshes[meshIndex].MeshName;
}

void AssimpLogStream::write(const char* message) {
    PLOG(m_pSeverity) << message;
}

void AssimpLoader::loadFile() {
    // Free resources
    m_meshIndices.clear();
    m_meshVertexData.clear();
    m_importer.FreeScene();

    // Set importer properties

    // Set Integer properties
    for (auto property : m_importerProperties.integerProperty) {
        m_importer.SetPropertyInteger(property.first.c_str(), property.second);
    }
    // Set Bool properties
    for (auto property : m_importerProperties.boolProperty) {
        m_importer.SetPropertyBool(property.first.c_str(), property.second);
    }
    // Set Float properties
    for (auto property : m_importerProperties.floatProperty) {
        m_importer.SetPropertyFloat(property.first.c_str(), property.second);
    }
    // Set String properties
    for (auto property : m_importerProperties.stringProperty) {
        m_importer.SetPropertyString(property.first.c_str(), property.second);
    }
    // Set Matrix properties
    for (auto property : m_importerProperties.matrixProperty) {
        m_importer.SetPropertyMatrix(property.first.c_str(), property.second);
    }
    // Set Pointer properties
    for (auto property : m_importerProperties.pointerProperty) {
        m_importer.SetPropertyPointer(property.first.c_str(), property.second);
    }

    // Load and process file
    m_scene = m_importer.ReadFile(m_filePath,
                                  m_importerProperties.postProcessingFlags);

    // Pre-allocate memory space
    m_meshVertexData.resize(m_scene->mNumMeshes);
    m_meshIndices.resize(m_scene->mNumMeshes);
    for (int i = 0; i < m_scene->mNumMeshes; i++) {
        m_meshVertexData[i].reserve(m_scene->mMeshes[i]->mNumVertices);
        m_meshIndices[i].reserve(m_scene->mMeshes[i]->mNumFaces * 3);
    }

    // Generate interleaved vertex data buffer
    for (int i = 0; i < m_scene->mNumMeshes; i++) {
        auto mesh = m_scene->mMeshes[i];

        // Generate Vertex Data
        if (!mesh->HasPositions() || !mesh->HasFaces()) continue;

        // Only 1 texture coord supported
        // Get first texture coord set defined.
        bool hasTextureSet = false;
        int textureSet = -1;
        for (int coordIdx = 0; coordIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS;
             coordIdx++) {
            if (mesh->HasTextureCoords(coordIdx)) {
                hasTextureSet = true;
                textureSet = coordIdx;
                break;
            }
        }

        // Only 1 per vertex color supported
        // Get first color set defined.
        bool hasColorSet = false;
        int colorSet = -1;
        for (int colorIdx = 0; colorIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS;
             colorIdx++) {
            if (mesh->HasVertexColors(colorIdx)) {
                hasColorSet = true;
                textureSet = colorIdx;
            }
        }

        for (int j = 0; j < mesh->mNumVertices; j++) {
            aiVector3D textCoord;
            if (hasTextureSet) textCoord = mesh->mTextureCoords[textureSet][j];
            aiColor4D color{1, 1, 1, 1};
            if (hasColorSet) color = mesh->mColors[textureSet][j];

            aiVector3D normal;
            if (mesh->HasNormals()) normal = mesh->mNormals[j];

            aiVector3D tangent;
            aiVector3D bitangent;
            if (mesh->HasTangentsAndBitangents()) {
                tangent = mesh->mTangents[j];
                bitangent = mesh->mBitangents[j];
            }

            m_meshVertexData[i].push_back(AssimpLoaderVertex{
                mesh->mVertices[j],
                normal,
                aiVector2D(textCoord.x,
                           textCoord.y),  // 3D textCoord not supported
                tangent,
                bitangent,
                color,
            });
        }
    }

    // Generate interleaved indices buffer
    for (int i = 0; i < m_scene->mNumMeshes; i++) {
        auto mesh = m_scene->mMeshes[i];
        // Generate indices data
        for (int j = 0; j < mesh->mNumFaces; j++) {
            // Assuming all faces have been triangulated
            m_meshIndices[i].push_back(mesh->mFaces[j].mIndices[0]);
            m_meshIndices[i].push_back(mesh->mFaces[j].mIndices[1]);
            m_meshIndices[i].push_back(mesh->mFaces[j].mIndices[2]);
        }
    }

    if (m_scene == nullptr) {
        m_fileLoaded = false;
        std::runtime_error("AssimpLoader failed to load file \"" + m_filePath +
                           "\". " + m_importer.GetErrorString());
    }
    m_fileLoaded = true;
}

void AssimpLoader::loadFile(std::string_view filePath) {
    m_filePath = filePath;
    loadFile();
}

std::tuple<size_t, void*, VertexLayout> AssimpLoader::getMeshVertexBuffer(
    unsigned int meshIndex) {
    VertexLayout vLayout;

    vLayout.stride = sizeof(AssimpLoaderVertex);

    vLayout.attributes.push_back(
        VertexAttribute{0, 3, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(AssimpLoaderVertex, vertice)});
    vLayout.attributes.push_back(
        VertexAttribute{1, 3, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(AssimpLoaderVertex, normal)});
    vLayout.attributes.push_back(
        VertexAttribute{2, 2, GL_FLOAT, VK_FORMAT_R32G32_SFLOAT, GL_FALSE,
                        offsetof(AssimpLoaderVertex, textureCoord)});
    vLayout.attributes.push_back(
        VertexAttribute{3, 3, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(AssimpLoaderVertex, color)});
    vLayout.attributes.push_back(
        VertexAttribute{4, 3, GL_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, GL_FALSE,
                        offsetof(AssimpLoaderVertex, tangent)});
    vLayout.attributes.push_back(
        VertexAttribute{5, 4, GL_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, GL_FALSE,
                        offsetof(AssimpLoaderVertex, bitangent)});

    return std::tuple<size_t, void*, VertexLayout>(
        m_scene->mMeshes[meshIndex]->mNumVertices * sizeof(AssimpLoaderVertex),
        m_meshVertexData[meshIndex].data(), vLayout);
}
size_t AssimpLoader::getMeshNumVertex(unsigned int meshIndex) {
    return m_meshVertexData[meshIndex].size();
}
unsigned int* AssimpLoader::getMeshIndicesBuffer(unsigned int meshIndex) {
    return m_meshIndices[meshIndex].data();
}
size_t AssimpLoader::getMeshNumIndices(unsigned int meshIndex) {
    return m_meshIndices[meshIndex].size();
}
std::string_view AssimpLoader::getMeshName(unsigned int meshIndex) {
    return m_scene->mMeshes[meshIndex]->mName.data;
}
}  // namespace v3d
