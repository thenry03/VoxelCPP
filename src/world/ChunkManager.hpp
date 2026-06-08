#pragma once

#include "Chunk.hpp"

#include <glm/glm.hpp>

#include <cstddef>
#include <functional>
#include <unordered_map>

// --- Hash functor for glm::ivec3 ---
// Required by std::unordered_map to index chunks by position
struct IVec3Hash
{
    std::size_t operator()(const glm::ivec3 &v) const
    {
        // Convert each component to a large hash value
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        // Combine with XOR and bit shifts to reduce collisions
        return h1 ^ (h2 << 16) ^ (h3 << 32);
    }
};

class ChunkManager
{
public:
    // --- Lifecycle ---
    ChunkManager()  = default;
    ~ChunkManager() = default;

    // --- Set and get chunks ---
    void addChunk(Chunk chunk);
    const Chunk &getChunk(const glm::ivec3 &chunkPosition) const;
    bool hasChunk(const glm::ivec3 &chunkPosition) const;

    // --- Iteration ---
    // Read-only iteration that does not leak the underlying container type
    // Callers use a range-based for and never name the map nor IVec3Hash
    using const_iterator = std::unordered_map<glm::ivec3, Chunk, IVec3Hash>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;

private:
    // --- Private attributes ---
    // HashMap so lookups by position are O(1) on average
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> m_chunkDatabase;
};