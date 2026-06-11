#pragma once

#include "Constants.hpp"

#include <glm/glm.hpp>

class Input;

class Camera
{
public:
    // --- Lifecycle ---
    Camera(glm::vec3 position = Config::Camera::START_POSITION,
           float yaw = Config::Camera::YAW,
           float pitch = Config::Camera::PITCH);
    ~Camera() = default;

    // --- Update ---
    void update(const Input &input, float deltaTime);

    // --- Queries ---
    glm::vec3 getPlayerPosition() const;
    glm::mat4 getViewMatrix() const;

private:
    // --- Private attributes ---
    glm::vec3 m_position;
    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_yaw;   // How much the user is looking left or right
    float m_pitch; // How much the user is looking up or down
    float m_moveSpeed = Config::Camera::MOVE_SPEED;
    float m_mouseSensitivity = Config::Camera::MOUSE_SENSITIVITY;
};