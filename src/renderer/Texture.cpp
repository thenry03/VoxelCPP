#define STB_IMAGE_IMPLEMENTATION

#include "Texture.hpp"

#include <stb_image.h>

#include <stdexcept>

// ==========================================
// 1. LIFECYCLE
// ==========================================
Texture::Texture(const std::string &atlasPath)
{
    // =========================
    // 1. Flip
    // =========================
    stbi_set_flip_vertically_on_load(true);

    // =========================
    // 2. Generate object
    // =========================
    glGenTextures(1, &m_textureID);

    // =========================
    // 3. Bind object
    // =========================
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // =========================
    // 4. Load with stb_image
    // =========================
    // All textures have transparency channel (PNG)
    // No need to set different logic depending on number of channels
    int width, height, channels;
    unsigned char *data = stbi_load(atlasPath.c_str(),
                                    &width,
                                    &height,
                                    &channels,
                                    0);
    if (!data)
        throw std::runtime_error(std::string("Could not load texture atlas: ")
        + stbi_failure_reason());
    if (channels != 4)
    {
        stbi_image_free(data);
        throw std::runtime_error("Texture format must be PNG.");
    }

    // Load
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data);

    // =========================
    // 5. Filter configuration
    // =========================
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // =========================
    // 6. Mipmaps
    // =========================
    glGenerateMipmap(GL_TEXTURE_2D);

    // =========================
    // 7. Free data
    // =========================
    stbi_image_free(data);
}

Texture::~Texture()
{
    if (m_textureID != 0)
        glDeleteTextures(1, &m_textureID);
}

// ==========================================
// 2. ACTIVATION
// ==========================================
void Texture::bind(unsigned int slot) const
{
    // Activates the given texture unit before binding
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}