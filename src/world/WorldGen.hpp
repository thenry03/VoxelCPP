#pragma once

#include "Chunk.hpp"

#include <glm/glm.hpp>

#include <cstdint>

// --- World generation ---
namespace WorldGen
{
    // Define the generation types
    enum class GenerationType : uint8_t
    {
        Simplex2D,
        Simplex3D
    };

    // Signature accepts generation type
    Chunk generateChunk(const glm::ivec3 &chunkPosition, GenerationType type);
}