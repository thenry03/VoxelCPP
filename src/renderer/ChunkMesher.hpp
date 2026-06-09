#pragma once

#include "../world/Chunk.hpp"
#include "Vertex.hpp"

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>

// --- Definition ---
// Groups all generated geometry to upload directly to GPU
struct ChunkMesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

// --- Chunk neighbours ---
// Needed to prevent race conditions when meshing a chunk
struct ChunkNeighbourhood
{
    Chunk center;
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> neighbours;
};

namespace ChunkMesher
{
    // --- Public interface ---
    // Constant reference (&) of Chunk is passed
    // Prevents heavy copies
    // Naive mesh: emits all 6 faces for every solid voxel
    // Simple, but wasteful
    ChunkMesh generateDumbMesh(const Chunk &chunk);
    // Culled mesh: skips faces shared with solid neighbours
    // Requires ChunkManager to resolve faces at chunk boundaries
    ChunkMesh generateCulledMesh(const ChunkNeighbourhood &neighbourhood);
}