#pragma once

#include "Shader.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

class SunRenderer
{
public:
    // --- Lifecycle ---
    SunRenderer();
    ~SunRenderer();

    // --- RAII ---
    SunRenderer(const SunRenderer &) = delete;
    SunRenderer &operator=(const SunRenderer &) = delete;

    // --- Public methods ---
    void draw(const glm::vec3 &sunPosition,
              const glm::vec3 &sunColor,
              float size,
              const glm::mat4 &view,
              const glm::mat4 &projection) const;

private:
    // --- Private attributes ---
    GLuint m_VAO;         // Vertex Array Object
    GLuint m_VBO;         // Vertex Buffer Object
    GLuint m_EBO;         // Element Buffer Object
    GLsizei m_indexCount; // How many indices per frame?
    Shader m_sunShader;
};