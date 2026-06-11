#pragma once

#include <glm/glm.hpp>

#include <cstddef>

namespace Config
{
    // --- Core ---
    namespace Window
    {
        constexpr int WIDTH = 800;
        constexpr int HEIGHT = 600;
    }

    namespace Camera
    {
        constexpr float MOVE_SPEED = 150.0f;
        constexpr float MOUSE_SENSITIVITY = 0.085f;
        inline const glm::vec3 START_POSITION = glm::vec3(0.0f, 140.0f, 5.0f);
        constexpr float YAW = -90.0f;
        constexpr float PITCH = 0.0f;
        constexpr float PITCH_LIMIT = 89.0f;
        constexpr float NEAR_PLANE = 0.2f;
        // Must reach the diagonal corner of the render distance:
        // RENDER_DISTANCE (64) * CHUNK_WIDTH (16) * sqrt(2) ~ 1450 blocks
        constexpr float FAR_PLANE = 3000.0f;
        constexpr float FOV = 45.0f;
    }

    namespace Input
    {
        constexpr float CURSOR_SETTLE_DELAY = 0.15f;
    }

    // --- Renderer ---
    namespace Renderer
    {
        constexpr float SKY_R = 0.53f;
        constexpr float SKY_G = 0.81f;
        constexpr float SKY_B = 0.98f;
        constexpr float SKY_A = 1.0f;

        constexpr float TEXTURE_UV_SIZE = 0.125f;
    }

    // --- World ---
    namespace World
    {
        constexpr std::size_t CHUNK_WIDTH = 16;
        constexpr std::size_t CHUNK_DEPTH = 16;
        constexpr std::size_t CHUNK_HEIGHT = 256;
        constexpr std::size_t CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT;

        constexpr unsigned int RENDER_DISTANCE = 32;

        // World generation: master seed
        constexpr int WORLD_SEED = static_cast<int>(10210511411511632ULL % 2147483647ULL);
        // FBm octaves shared by the plains and mountains surface noises
        constexpr int WORLD_NOISE_OCTAVES = 4;

        // Terrain type selector: blends plains <-> mountains
        // Very low frequency so each biome spans many chunks; +1 seed decorrelates
        // it from the surface noises (FastNoiseLite hashes the seed)
        constexpr float SELECTOR_NOISE_FREQUENCY = 0.0012f;

        // Terrain warp: introduces controlled randomness
        // Three octaves so as to not completely break terrain
        // Very low frequency so the coordinates change in a slow, gentle way
        constexpr float WARP_NOISE_OCTAVES = 3;
        constexpr float WARP_NOISE_FREQUENCY = 0.0015f;

        // Mean ground level, as a fraction of CHUNK_HEIGHT
        constexpr float TERRAIN_BASE = 0.375f;     // 96 blocks
        constexpr float SEA_LEVEL_FACTOR = 0.350f; // ~90 blocks

        // Plains: broad, gentle rolls with little relief
        constexpr float PLAINS_NOISE_FREQUENCY = 0.010f;
        constexpr float PLAINS_TERRAIN_AMPLITUDE = 0.030f; // ~8 blocks

        // Mountains: large masses with strong relief
        constexpr float MOUNTAINS_NOISE_FREQUENCY = 0.006f;
        constexpr float MOUNTAINS_TERRAIN_AMPLITUDE = 0.340f; // ~72 blocks

        // Rivers
        constexpr float RIVERS_NOISE_FREQUENCY = 0.001f;

        // Dirt layers between the grass top and the stone below
        constexpr int DIRT_DEPTH = 3;
    }
}