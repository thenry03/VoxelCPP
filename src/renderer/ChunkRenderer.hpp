#pragma once

#include "ChunkMesher.hpp"

#include <glad/glad.h>

class ChunkRenderer
{
public:
    // --- Lifecycle ---
    ChunkRenderer();
    ~ChunkRenderer();

    // --- RAII ---
    ChunkRenderer(const ChunkRenderer&)            = delete;
    ChunkRenderer& operator=(const ChunkRenderer&) = delete;

    // --- Public methods ---
    void updateMesh(const ChunkMesh& mesh);
    void draw() const;

private:
    // --- Private attributes ---
    GLuint m_VAO;         // Vertex Array Object
    GLuint m_VBO;         // Vertex Buffer Object
    GLuint m_EBO;         // Element Buffer Object
    GLsizei m_indexCount; // How many indices per frame?
};