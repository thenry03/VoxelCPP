#pragma once

#include <glm/glm.hpp>

// Window
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

// Camera
constexpr float CAM_MOVE_SPEED = 5.0f;
constexpr float CAM_MOUSE_SENSITIVITY = 0.075f;
inline const glm::vec3 CAM_START_POSITION = glm::vec3(0.0f, 0.0f, 5.0f);
constexpr float CAM_YAW = -90.0f;
constexpr float CAM_PITCH = 0.0f;
constexpr float CAM_PITCH_LIMIT = 89.0f;
constexpr float CAM_NEAR = 0.1f;
constexpr float CAM_FAR = 100.0f;
constexpr float CAM_FOV = 45.0f;

// Input
constexpr float CURSOR_SETTLE_DELAY = 0.15f;