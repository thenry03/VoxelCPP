#pragma once

#include "../chunks/Chunk.hpp"

#include <glm/glm.hpp>

class TerrainShaper;

// --- Definition ---
// Groups all generated flora to stamp in chunk
namespace Flora
{
    void stampFloraInChunk(Chunk &chunk,
                           const glm::ivec3 &chunkPosition,
                           const TerrainShaper &terrainShaper,
                           int seed);
}