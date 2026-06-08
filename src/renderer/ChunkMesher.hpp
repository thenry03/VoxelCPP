#pragma once

#include "../world/Chunk.hpp"
#include "../world/ChunkManager.hpp"
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
    // Several meshing algorithms
    ChunkMesh generateDumbMesh(const Chunk &chunk);
    ChunkMesh generateCulledMesh(const Chunk &chunk, const ChunkManager &chunkManager);
}