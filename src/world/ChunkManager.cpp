#include "ChunkManager.hpp"

#include <utility>

// ==========================================
// 1. PUBLIC METHODS
// ==========================================
void ChunkManager::addChunk(Chunk chunk)
{
    // Store the chunk in the database, keyed by its position
    m_chunkDatabase.emplace(chunk.getPosition(), std::move(chunk));
}

const Chunk &ChunkManager::getChunk(const glm::ivec3 &position) const
{
    return m_chunkDatabase.at(position);
}

bool ChunkManager::hasChunk(const glm::ivec3 &position) const
{
    return m_chunkDatabase.count(position) > 0;
}

ChunkManager::const_iterator ChunkManager::begin() const
{
    return m_chunkDatabase.begin();
}

ChunkManager::const_iterator ChunkManager::end() const
{
    return m_chunkDatabase.end();
}