#pragma once

#include <array>

#include <GLFW/glfw3.h>

class Input
{
public:
    // --- Lifecycle ---
    Input(GLFWwindow *window);
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
    // --- Private methods ---
    static void keyCallback(GLFWwindow *window,
                            int key,
                            int scancode,
                            int action,
                            int mods);

    static void mouseCallback(GLFWwindow *window,
                              double xPos,
                              double yPos);

    // --- Private attributes ---
    GLFWwindow *m_window = nullptr;
    std::array<bool, GLFW_KEY_LAST + 1> m_keys{};
    std::array<bool, GLFW_KEY_LAST + 1> m_keysPrev{};
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    bool m_firstMouse = true; // Stops first frame problems
};