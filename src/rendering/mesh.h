#pragma once

#include <cstddef>

namespace objl {
class Mesh;
}  // namespace objl

namespace v3d {

class Mesh {
   public:
    // virtual ~Mesh() = default;
    virtual void draw() = 0;
};

class MeshOpenGL : public Mesh {
   public:
    MeshOpenGL();
    MeshOpenGL(objl::Mesh &mesh);
    ~MeshOpenGL();

    void draw() override;

   private:
    unsigned int m_VBO, m_VAO, m_EBO;
    size_t m_numIndices;
};
}  // namespace v3d
