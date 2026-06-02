#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Constants.hpp"
#include "Input.hpp"

class Camera
{
public:
    // --- Lifecycle ---
    Camera(glm::vec3 position = CAM_START_POSITION,
           float yaw = CAM_YAW,
           float pitch = CAM_PITCH);
    ~Camera();

    // --- Update ---
    void update(const Input &input, float deltaTime);

    // --- Matrix query ---
    glm::mat4 getViewMatrix() const;

private:
    // --- Private attributes ---
    glm::vec3 m_position;
    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_yaw;   // How much the user is looking left or right
    float m_pitch; // How much the user is looking up or down
    float m_moveSpeed = CAM_MOVE_SPEED;
    float m_mouseSensitivity = CAM_MOUSE_SENSITIVITY;
};