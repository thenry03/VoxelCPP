#pragma once

#include "../world/chunks/Chunk.hpp"
#include "../world/chunks/ChunkNeighbourhood.hpp"
#include "Vertex.hpp"

#include <vector>

// --- Definition ---
// Groups all generated geometry to upload directly to GPU
struct ChunkMesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
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