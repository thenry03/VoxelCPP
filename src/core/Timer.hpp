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
    // Absolute times kept as double (glfwGetTime returns double; float loses
    // precision over a long-running session)
    double m_currentTime = 0.0;
    double m_lastTime = 0.0;
    double m_lastFPSTime = 0.0;
    // Deltas and counters
    float m_deltaTime = 0.0f;
    unsigned int m_nbFrames = 0;
    float m_fps = 0.0f; // Last computed FPS, held between recalculations
};