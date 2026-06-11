#include "SunRenderer.hpp"

// ==========================================
// 1. LIFECYCLE
// ==========================================
SunRenderer::SunRenderer()
    : m_sunShader("shaders/sun.vert", "shaders/sun.frag")
{
    m_indexCount = 6;

    // =========================
    // 1. Define data
    // =========================
    float sunVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f};

    unsigned int sunIndices[] = {
        0, 1, 2,
        2, 3, 0};

    // =========================
    // 2. Generate objects
    // =========================
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // =========================
    // 3. Bind objects
    // =========================
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // =========================
    // 4. Upload objects
    // =========================
    // Upload vertices to GPU using static drawing
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(sunVertices),
                 sunVertices,
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(sunIndices),
                 sunIndices,
                 GL_STATIC_DRAW);

    // =========================
    // 5. Vertex attributes
    // =========================
    // Only position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // =========================
    // 6. Unbind objects
    // =========================
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

SunRenderer::~SunRenderer()
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
void SunRenderer::draw(const glm::vec3 &sunPosition,
                       const glm::vec3 &sunColor,
                       float sunSize,
                       const glm::mat4 &view,
                       const glm::mat4 &projection) const
{
    // Bind its shader
    m_sunShader.bind();

    // Pass uniforms
    m_sunShader.setVec3("sunPosition", sunPosition);
    m_sunShader.setVec3("sunColor", sunColor);
    m_sunShader.setFloat("sunSize", sunSize);
    m_sunShader.setMat4("view", view);
    m_sunShader.setMat4("projection", projection);

    // Draw
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    
    // Unbind VAO
    glBindVertexArray(0);
}