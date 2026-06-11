#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

// ==========================================
// 1. HELPER METHODS
// ==========================================
static std::string readFile(const std::string &shaderPath)
{
    // Open the shader source file on disk
    std::ifstream file(shaderPath);

    if (!file.is_open())
        throw std::runtime_error("Could not load shader file.");

    // Dump the whole file into a string buffer
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// Queries the compilation status of a shader object
// Throws std::runtime_error with the driver's info log on failure
// Cleans up both shader handles before throwing
static void compilationCheck(GLuint shader, GLuint vertexShader, GLuint fragmentShader)
{
    GLint compilationSuccessful;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccessful);
    if (!compilationSuccessful)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        // Clean up both shaders before throwing
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error(std::string("Could not compile shader:\n") + infoLog);
    }
}

// Queries the link status of a shader program
// Throws std::runtime_error with the driver's info log on failure
// Cleans up the program and both shader handles before throwing
static void linkCheck(GLuint shader, GLuint vertexShader, GLuint fragmentShader)
{
    GLint linkSuccessful;
    glGetProgramiv(shader, GL_LINK_STATUS, &linkSuccessful);
    if (!linkSuccessful)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(shader, 512, nullptr, infoLog);

        // Clean up the program and both shaders before throwing
        glDeleteProgram(shader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error(std::string("Could not link shader program:\n") + infoLog);
    }
}

// ==========================================
// 2. LIFECYCLE
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
// 3. PUBLIC METHODS
// ==========================================
void Shader::bind() const
{
    glUseProgram(m_programID);
}

void Shader::setInt(const std::string &name, int value) const
{
    GLint location = getUniformLocation(name);
    if (location == -1)
        return;

    glUniform1i(location, value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    GLint location = getUniformLocation(name);
    if (location == -1)
        return;

    glUniform1f(location, value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    GLint location = getUniformLocation(name);
    if (location == -1)
        return;

    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    GLint location = getUniformLocation(name);
    if (location == -1)
        return;

    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) const
{
    GLint location = getUniformLocation(name);
    if (location == -1)
        return;

    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const
{
    GLint location = getUniformLocation(name);
    if (location == -1)
        return;

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

// ==========================================
// 4. PRIVATE METHODS
// ==========================================
GLint Shader::getUniformLocation(const std::string &name) const
{
    // If uniform already in cache, return it
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
        return m_uniformLocationCache[name];

    // glGetUniformLocation requires a C-string; .c_str() provides the raw pointer
    GLint location = glGetUniformLocation(m_programID, name.c_str());

    if (location == -1)
        return -1;

    // Store in cache for next frame
    m_uniformLocationCache[name] = location;

    return location;
}