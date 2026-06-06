#include "Chunk.hpp"

// ==========================================
// 1. LIFECYCLE
// ==========================================
// Initializes every single block to AIR
Chunk::Chunk(const glm::ivec3 &position)
    : m_chunkPosition(position), m_blockIDs(Config::World::CHUNK_VOLUME, 0)
{
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
uint8_t Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= static_cast<int>(Config::World::CHUNK_WIDTH) ||
        z < 0 || z >= static_cast<int>(Config::World::CHUNK_DEPTH) ||
        y < 0 || y >= static_cast<int>(Config::World::CHUNK_HEIGHT))
    {
        return 0;
    }

    return m_blockIDs[getIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, uint8_t blockID)
{
    if (x < 0 || x >= static_cast<int>(Config::World::CHUNK_WIDTH) ||
        z < 0 || z >= static_cast<int>(Config::World::CHUNK_DEPTH) ||
        y < 0 || y >= static_cast<int>(Config::World::CHUNK_HEIGHT))
    {
        return;
    }

    m_blockIDs[getIndex(x, y, z)] = blockID;
}