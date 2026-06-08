#include "Input.hpp"

#include "Constants.hpp"
#include "Window.hpp"

// ==========================================
// 1. LIFECYCLE
// ==========================================
Input::Input(Window &window)
{
    // =========================
    // 1. Mouse capture
    // =========================
    GLFWwindow *handle = window.getNativeWindow();

    // Disable cursor and set raw mouse motion
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // Reference seed
    double startX, startY;
    glfwGetCursorPos(handle, &startX, &startY);
    m_mouseX = static_cast<float>(startX);
    m_mouseY = static_cast<float>(startY);
    m_captureTime = glfwGetTime();

    // =========================
    // 2. Sub to dispatcher
    // =========================
    EventDispatcher &eventDispatcher = window.getEventDispatcher();

    // Sub onKey to dispatcher
    eventDispatcher.onKey([this](int key, int action)
                          {
        if (action == GLFW_PRESS)
            m_keys[key] = true;
        else if (action == GLFW_RELEASE)
            m_keys[key] = false; });

    // Sub onMouse to dispatcher
    eventDispatcher.onMouse([this](float xPos, float yPos)
                            {
        if (m_mouseReady)
        {
            m_mouseDeltaX = xPos - m_mouseX;
            m_mouseDeltaY = m_mouseY - yPos;
        }

         m_mouseX = xPos;
    m_mouseY = yPos; });
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

    // Trust deltas only when cursor settles
    if (!m_mouseReady && (glfwGetTime() - m_captureTime) > Config::Input::CURSOR_SETTLE_DELAY)
        m_mouseReady = true;
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