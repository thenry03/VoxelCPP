#pragma once

#include <glm/glm.hpp>
#include <cstddef>

namespace Config
{
    // ==========================================
    // 1. CORE
    // ==========================================
    namespace Window
    {
        constexpr int WIDTH = 800;
        constexpr int HEIGHT = 600;
    }

    namespace Camera
    {
        constexpr float MOVE_SPEED = 5.0f;
        constexpr float MOUSE_SENSITIVITY = 0.075f;
        inline const glm::vec3 START_POSITION = glm::vec3(0.0f, 0.0f, 5.0f);
        constexpr float YAW = -90.0f;
        constexpr float PITCH = 0.0f;
        constexpr float PITCH_LIMIT = 89.0f;
        constexpr float NEAR_PLANE = 0.1f;
        constexpr float FAR_PLANE = 100.0f;
        constexpr float FOV = 45.0f;
    }

    namespace Input
    {
        constexpr float CURSOR_SETTLE_DELAY = 0.15f;
    }

    // ==========================================
    // 2. WORLD
    // ==========================================
    namespace World
    {
        // Renombramos conceptualmente a ejes X, Z, Y para que al escribir
        // los bucles del Mesher o Generador sea imposible confundirse.
        constexpr std::size_t CHUNK_WIDTH = 16;
        constexpr std::size_t CHUNK_DEPTH = 16;
        constexpr std::size_t CHUNK_HEIGHT = 16;

        // Volumen total del array plano del chunk
        constexpr std::size_t CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT;
    }
}