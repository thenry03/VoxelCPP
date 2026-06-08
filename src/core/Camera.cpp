#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

// ==========================================
// 1. LIFECYCLE
// ==========================================
Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : m_position(position), m_yaw(yaw), m_pitch(pitch)
{
    // Recalculate front vector from initial yaw and pitch
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void Camera::update(const Input &input, float deltaTime)
{
    // Scales velocity by elapsed time to ensure frame-rate independent movement
    float speed = m_moveSpeed * deltaTime;

    // Handle player movement
    if (input.isKeyPressed(GLFW_KEY_W))
        m_position += m_front * speed;
    if (input.isKeyPressed(GLFW_KEY_S))
        m_position -= m_front * speed;
    // Cross product between front and up = +X direction
    if (input.isKeyPressed(GLFW_KEY_A))
        m_position -= glm::normalize(glm::cross(m_front, m_up)) * speed;
    if (input.isKeyPressed(GLFW_KEY_D))
        m_position += glm::normalize(glm::cross(m_front, m_up)) * speed;
    // Up and down
    if (input.isKeyPressed(GLFW_KEY_SPACE))
        m_position += m_up * speed;
    if (input.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        m_position -= m_up * speed;

    // How much the cursor moved left or right
    m_yaw += input.getMouseDeltaX() * m_mouseSensitivity;
    // How much the cursor moved up or down
    m_pitch += input.getMouseDeltaY() * m_mouseSensitivity;

    // Limit pitch
    if (m_pitch > Config::Camera::PITCH_LIMIT)
        m_pitch = Config::Camera::PITCH_LIMIT;
    if (m_pitch < -Config::Camera::PITCH_LIMIT)
        m_pitch = -Config::Camera::PITCH_LIMIT;

    // Convert Euler angles to a direction vector using spherical projection
    // cos(pitch) flattens X and Z as the camera looks up or down
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);
}

glm::mat4 Camera::getViewMatrix() const
{
    // Camera position, camera direction and up vector
    return glm::lookAt(
        m_position,
        m_position + m_front,
        m_up);
}