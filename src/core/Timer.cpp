#include "Timer.hpp"

#include <GLFW/glfw3.h>

// ==========================================
// 1. LIFECYCLE
// ==========================================
Timer::Timer()
{
    m_lastTime = (float)glfwGetTime();
    m_lastFPSTime = m_lastTime;
}

Timer::~Timer()
{
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
float Timer::update()
{
    // Important to prevent differences between hardware
    // Distance = Speed × Delta Time
    m_currentTime = (float)glfwGetTime();
    m_deltaTime = m_currentTime - m_lastTime;
    // Important: last time should be update indepently from FPS
    // If not, WASD movement will break
    m_lastTime = m_currentTime;

    // Frame counter goes up
    m_nbFrames++;

    // Must separate deltaTime calculation from FPS time calculation
    float fpsElapsed = m_currentTime - m_lastFPSTime;

    if (fpsElapsed >= 1.0f)
    {
        // Recalculate FPS once per second and cache it
        m_fps = m_nbFrames / fpsElapsed;
        // Set frames to 0 and current time is now last frame's time
        m_nbFrames = 0.0f;
        m_lastFPSTime = m_currentTime;
    }

    // Return the cached value every frame
    return m_fps;
}

float Timer::getDeltaTime() const
{
    return m_deltaTime;
}