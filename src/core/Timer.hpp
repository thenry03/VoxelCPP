#pragma once

class Timer
{
public:
    // --- Lifecycle ---
    Timer();
    ~Timer();

    // --- Update ---
    float update();

    // --- Query ---
    float getDeltaTime() const;

private:
    // --- Private attributes ---
    float m_currentTime = 0.0f;
    float m_lastTime = 0.0f;
    float m_deltaTime = 0.0f;
    float m_nbFrames = 0.0f;
    float m_lastFPSTime = 0.0f;
    float m_fps = 0.0f; // Last computed FPS, held between recalculations
};