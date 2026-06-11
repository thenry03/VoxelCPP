#pragma once

#include <glm/glm.hpp>

class Sun
{
public:
    // --- Lifecycle ---
    Sun();
    ~Sun() = default;

    // --- Query ---
    glm::vec3 getSunPosition() const;
    glm::vec3 getSunColor() const;
    float getSunSize() const;
    float getSunLightEmitted() const;

private:
    // --- Private attributes ---
    glm::vec3 m_position; // Fixed position in the world
    glm::vec3 m_color;    // Fixed color
    float m_size;         // Apparent size (quad radius in world units)
    float m_lightEmitted; // Future Flood Fill, not used
};