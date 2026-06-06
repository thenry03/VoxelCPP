#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Shader
{
public:
    // --- Lifecycle ---
    Shader(const std::string &vertPath, const std::string &fragPath);
    ~Shader();

    // --- RAII ---
    Shader(const Shader &)            = delete;
    Shader &operator=(const Shader &) = delete;

    // --- Activation ---
    void bind()   const;
    void unbind() const;

    // --- Uniforms ---
    void setInt(const std::string &name, int value)               const;
    void setFloat(const std::string &name, float value)           const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat3(const std::string &name, const glm::mat3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;

private:
    // --- Private methods ---
    GLint getUniformLocation(const std::string& name) const;

    // --- Private attributes ---
    GLuint m_programID = 0;
    // Location cache
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
        
};