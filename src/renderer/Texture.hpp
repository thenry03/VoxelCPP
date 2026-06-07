#pragma once

#include <glad/glad.h>

#include <string>

class Texture
{
public:
    // --- Lifecycle ---
    Texture(const std::string& atlasPath);
    ~Texture();

    // --- RAII ---
    Texture(const Texture &)            = delete;
    Texture &operator=(const Texture &) = delete;

    // --- Activation ---
    void bind(unsigned int slot) const;

private:
    // --- Private attributes ---
    GLuint m_textureID = 0;
};