#pragma once

class Timer
{
public:
    // --- Lifecycle ---
    Timer();
    ~Timer();

    // --- Update ---
    void update();

    // --- Query ---
    float getDeltaTime() const;

private:
    // --- Private attributes ---
    float m_currentTime = 0.0f;
    float m_lastTime = 0.0f;
    float m_deltaTime = 0.0f;
};