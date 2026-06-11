#include "Sun.hpp"

// ==========================================
// 1. LIFECYCLE
// ==========================================
Sun::Sun()
    : m_position(0.0f, 3250.0f, 5000.0f),
      m_color(1.0f, 0.95f, 0.6f),
      m_size(75.0f),
      m_lightEmitted(1.0f)
{
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
glm::vec3 Sun::getSunPosition() const
{
    return m_position;
}

glm::vec3 Sun::getSunColor() const
{
    return m_color;
}

float Sun::getSunSize() const
{
    return m_size;
}

float Sun::getSunLightEmitted() const
{
    return m_lightEmitted;
}