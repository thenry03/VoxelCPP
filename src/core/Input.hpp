#pragma once

#include "EventDispatcher.hpp"

#include <GLFW/glfw3.h>

#include <array>

// Forward declaration
class Window;

class Input
{
public:
    // --- Lifecycle ---
    explicit Input(Window& window);
    ~Input();

    // --- RAII ---
    // Especially important:
    // Input(const Input&) = delete prevents Input b = a;
    // Input& operator=(const Input&) = delete prevents b = a;
    // (when b already exists)
    // What input is used if copies are allowed?
    Input(const Input &)            = delete;
    Input &operator=(const Input &) = delete;

    // --- Update ---
    void update();

    // --- State queries ---
    bool isKeyPressed(int key) const;
    bool isKeyJustPressed(int key) const;
    float getMouseDeltaX() const;
    float getMouseDeltaY() const;

private:
    // --- Private attributes ---
    std::array<bool, GLFW_KEY_LAST + 1> m_keys{};
    std::array<bool, GLFW_KEY_LAST + 1> m_keysPrev{};
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    double m_captureTime = 0.0f;
    bool m_mouseReady = false;
};