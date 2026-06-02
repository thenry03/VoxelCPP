#include "Input.hpp"

#include <GLFW/glfw3.h>

// ==========================================
// 1. LIFECYCLE
// ==========================================
Input::Input(GLFWwindow *window)
{
    // =========================
    // 1. Register callbacks
    // =========================
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    // Cursor must be hidden and captured inside window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // =========================
    // 2. Save window pointer
    // =========================
    m_window = window;
}

Input::~Input()
{
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void Input::update()
{
    // Current keys must become the previous ones
    m_keysPrev = m_keys;
    m_mouseDeltaX = 0.0f;
    m_mouseDeltaY = 0.0f;
}

bool Input::isKeyPressed(int key) const
{
    return m_keys[key];
}
bool Input::isKeyJustPressed(int key) const
{
    // Key is pressed and wasn't pressed last frame
    return m_keys[key] && !m_keysPrev[key];
}
float Input::getMouseDeltaX() const
{
    return m_mouseDeltaX;
}
float Input::getMouseDeltaY() const
{
    return m_mouseDeltaY;
}

// ==========================================
// 3. PRIVATE METHODS
// ==========================================
void Input::keyCallback(GLFWwindow *window,
                        int key,
                        int scancode,
                        int action,
                        int mods)
{
    // No modifiers
    (void)scancode;
    (void)mods;

    // Bridge the gap between GLFW C-style event system and object oriented Input class
    // This line retrieves the C++ Input instance from the GLFW window handle
    // This way the static callback can execute methods on the actual object
    Input *instance = static_cast<Input *>(glfwGetWindowUserPointer(window));

    if (!instance)
        return;

    if (action == GLFW_PRESS)
        instance->m_keys[key] = true;
    else if (action == GLFW_RELEASE)
        instance->m_keys[key] = false;
}

void Input::mouseCallback(GLFWwindow *window,
                          double xPos,
                          double yPos)
{
    Input *instance = static_cast<Input *>(glfwGetWindowUserPointer(window));

    if (!instance)
        return;

    if (instance->m_firstMouse)
    {
        // double is the type used by GLFW
        // Its precision is completely unnecesary
        // For that reason it is casted to float
        instance->m_mouseX = (float)xPos;
        instance->m_mouseY = (float)yPos;
        instance->m_firstMouse = false;
    }

    // If it is not the first mouse movement
    instance->m_mouseDeltaX = (float)xPos - instance->m_mouseX;
    instance->m_mouseDeltaY = instance->m_mouseY - (float)yPos;
    instance->m_mouseX = (float)xPos;
    instance->m_mouseY = (float)yPos;
}