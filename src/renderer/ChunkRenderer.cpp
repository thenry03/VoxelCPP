#include "ChunkRenderer.hpp"

// ==========================================
// 1. LIFECYCLE
// ==========================================
ChunkRenderer::ChunkRenderer()
{
    // =========================
    // 1. Generate objects
    // =========================
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // =========================
    // 2. Bind objects
    // =========================
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // =========================
    // 3. Vertex attributes
    // =========================
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // =========================
    // 4. Unbind objects
    // =========================
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ChunkRenderer::~ChunkRenderer()
{
    if (m_VAO != 0)
        glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0)
        glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0)
        glDeleteBuffers(1, &m_EBO);
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void ChunkRenderer::updateMesh(const ChunkMesh &mesh)
{
    // Get mesh indices count
    m_indexCount = static_cast<GLsizei>(mesh.indices.size());

    // If mesh if fully made up of air, just don't render
    if (m_indexCount == 0)
        return;

    // Bind VAO
    glBindVertexArray(m_VAO);

    // Upload vertices to GPU using dynamic drawing
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.vertices.size() * sizeof(Vertex),
                 mesh.vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.indices.size() * sizeof(unsigned int),
                 mesh.indices.data(),
                 GL_DYNAMIC_DRAW);

    // Unbind VBO only
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ChunkRenderer::draw() const
{
    // If no rendering required, no draw call
    if (m_indexCount == 0)
        return;

    // Draw
    // Bind VAO
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}