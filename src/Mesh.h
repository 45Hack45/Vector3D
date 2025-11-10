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

// template <typename MeshType>
class Mesh {
    friend class ModelManager;

   private:
    // std::unique_ptr<MeshType> m_instance;

    // static void internal_validate_constructor() {
    //     // Validate that the derived class implements the base constructor
    //     static_assert(
    //         std::is_constructible_v<MeshType, void*, size_t, VertexLayout,
    //                                 unsigned int*, size_t>,
    //         "Derived must have a constructor (void*, size_t, VertexLayout, "
    //         "unsigned int*, size_t)");
    // }

   protected:
    std::string m_name;
    Mesh() {}

   public:
    // Mesh() { internal_validate_constructor(); }
    // Mesh(void* vertexDataBuffer, size_t vertexDataBufferSize,
    //      VertexLayout vertexLayout, unsigned int* indicesBuffer,
    //      size_t indicesBufferSize) {
    //     // internal_validate_constructor();

    //     // // Create the Derived instance
    //     // m_instance = std::make_unique<MeshType>(
    //     //     vertexDataBuffer, vertexDataBufferSize, vertexLayout,
    //     //     indicesBuffer, indicesBufferSize);
    // }
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
