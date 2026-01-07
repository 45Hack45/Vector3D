#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace objl {
class Mesh;
}  // namespace objl

namespace v3d {

class ModelManager;

struct VertexAttribute {
    uint32_t location;    // layout location in shader
    uint32_t components;  // number of components (e.g. 3 for vec3)
    uint32_t glType;      // GL_FLOAT, GL_INT etc.
    uint32_t
        vkFormat;  // VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT etc.
    bool normalized;
    size_t offset;  // byte offset in struct
};

struct VertexLayout {
    size_t stride;
    std::vector<VertexAttribute> attributes;
};

class Mesh {
    friend class ModelManager;

   private:
   protected:
    std::string m_name;
    Mesh() {}

   public:
    virtual ~Mesh() = default;

    virtual void draw() const = 0;
    std::string_view getName() const { return m_name; }
};

class MeshOpenGL : public Mesh {
   public:
    MeshOpenGL() = delete;
    MeshOpenGL(void* vertexDataBuffer, size_t vertexDataBufferSize,
               VertexLayout vertexLayout, unsigned int* indicesBuffer,
               size_t indicesBufferSize);
    MeshOpenGL(objl::Mesh& mesh);
    ~MeshOpenGL() override;

    void draw() const override;

   private:
    unsigned int m_VBO = 0, m_VAO = 0, m_EBO = 0;
    size_t m_numIndices = 0;
};
}  // namespace v3d
