
#include "rendering/mesh.h"

#include <glad/glad.h>
#include <plog/Log.h>

#include "OBJ-Loader-master/Source/OBJ_Loader.h"
#include "rendering/graphics_backend.h"
#include "rendering/opengl_backend.h"

namespace v3d {

MeshOpenGL::MeshOpenGL() {
}

MeshOpenGL::MeshOpenGL(objl::Mesh &mesh) {
    // Send vertex data to GPU

    // Generate buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Load data into vertex/index buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), mesh.Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    m_numIndices = mesh.Indices.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), mesh.Indices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Vertex Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);  // unbind VAO

    PLOGI << "Buffer handles: " << m_VBO << "; " << m_VAO << "; " << m_EBO << "\n";
};

MeshOpenGL::~MeshOpenGL() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
};

void MeshOpenGL::draw() {
    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
}  // namespace v3d
