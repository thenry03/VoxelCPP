#pragma once

#include "Chunk.hpp"

#include <glm/glm.hpp>

#include <unordered_map>

// --- Chunk neighbourhood ---
// Needed to prevent race conditions when meshing a chunk
struct ChunkNeighbourhood
{
    Chunk center;
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> neighbours;
};