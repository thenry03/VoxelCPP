#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// ==========================================
// HELPER METHODS
// ==========================================
static std::string readFile(const std::string &shaderPath)
{
    // Read information from file in disk
    std::ifstream file(shaderPath);

    if (!file.is_open())
        throw std::runtime_error("Could not load shader file.");

    // Read and write information in text memory buffer
    std::stringstream buffer;
    buffer << file.rdbuf();

    // Return a string object
    return buffer.str();
}

static void compilationCheck(GLuint shader, GLuint vertexShader, GLuint fragmentShader)
{
    // Create a variable to store result
    GLint compilationSuccessful;
    // Store result in said variable
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccessful);
    // Check if compilation resulted successful
    if (!compilationSuccessful)
    {
        // Store compilation error in buffer
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        // Delete both vertex and fragment shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error(std::string("Could not compile shader:\n") + infoLog);
    }
}

static void linkCheck(GLuint shader, GLuint vertexShader, GLuint fragmentShader)
{
    // Create a variable to store result
    GLint linkSuccessful;
    // Store result in said variable
    glGetProgramiv(shader, GL_LINK_STATUS, &linkSuccessful);
    // Check if link resulted successful
    if (!linkSuccessful)
    {
        // Store link error in buffer
        GLchar infoLog[512];
        glGetProgramInfoLog(shader, 512, nullptr, infoLog);

        // Delete program and both vertex and fragment shader
        glDeleteProgram(shader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error(std::string("Could not link shader program:\n") + infoLog);
    }
}

GLint Shader::getUniformLocation(const std::string &name) const
{
    // If uniform already in cache, return it
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
        return m_uniformLocationCache[name];

    // Ask GPU using .c_str() so it compiles with no errors
    GLint location = glGetUniformLocation(m_programID, name.c_str());

    if (location == -1)
    {
        std::cerr << "Warning: uniform '" << name << "' not found.\n";
        return -1;
    }

    // Store in cache for next frame
    m_uniformLocationCache[name] = location;
    return location;
}

// ==========================================
// 1. LIFECYCLE
// ==========================================
Shader::Shader(const std::string &vertPath, const std::string &fragPath)
{
    // =========================
    // 1. Set source
    // =========================
    std::string vertSource = readFile(vertPath);
    std::string fragSource = readFile(fragPath);

    // =========================
    // 2. Creation
    // =========================
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // =========================
    // 3. Shader source
    // =========================
    const char *vertSourceStr = vertSource.c_str();
    const char *fragSourceStr = fragSource.c_str();
    glShaderSource(vertexShader, 1, &vertSourceStr, nullptr);
    glShaderSource(fragmentShader, 1, &fragSourceStr, nullptr);

    // =========================
    // 4. Compilation
    // =========================
    glCompileShader(vertexShader);
    compilationCheck(vertexShader, vertexShader, fragmentShader);
    glCompileShader(fragmentShader);
    compilationCheck(fragmentShader, vertexShader, fragmentShader);

    // =========================
    // 5. Create program
    // =========================
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);

    // =========================
    // 6. Link program
    // =========================
    glLinkProgram(m_programID);
    linkCheck(m_programID, vertexShader, fragmentShader);

    // =========================
    // 7. Free memory
    // =========================
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_programID);
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void Shader::bind() const
{
    glUseProgram(m_programID);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void Shader::setInt(const std::string &name, int value) const
{
    int location = getUniformLocation(name);
    if (location == -1)
        return;

    // Send uniform
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    int location = getUniformLocation(name);
    if (location == -1)
        return;

    // Send uniform
    glUniform1f(location, value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    int location = getUniformLocation(name);
    if (location == -1)
        return;

    // Send uniform
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    int location = getUniformLocation(name);
    if (location == -1)
        return;

    // Send uniform
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) const
{
    int location = getUniformLocation(name);
    if (location == -1)
        return;

    // Send uniform
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const
{
    int location = getUniformLocation(name);
    if (location == -1)
        return;

    // Send uniform
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}