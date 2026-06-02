#include "Timer.hpp"

#include <GLFW/glfw3.h>

// ==========================================
// 1. LIFECYCLE
// ==========================================
Timer::Timer()
{
    m_lastTime = (float)glfwGetTime();
}

Timer::~Timer()
{
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void Timer::update()
{
    // Important to prevent differences between hardware
    // Distance = Speed × Delta Time
    m_currentTime = (float)glfwGetTime();
    m_deltaTime = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;
}

float Timer::getDeltaTime() const
{
    return m_deltaTime;
}