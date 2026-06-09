#pragma once

#include "../renderer/ChunkMesher.hpp"
#include "Chunk.hpp"

#include <glm/glm.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

// --- Lifecycle States ---
// The 5 lifecycle states
enum class ChunkState : uint8_t
{
    Requested,  // Generation requested
    Generating, // Noise phase
    Meshing,    // Meshing and culling
    Ready,      // Ready to render
    Unloading   // Unload pending
};

class ChunkManager
{
public:
    // --- Structure package ---
    // Atomic unit to group data and state
    // Entries must remain synchronized
    struct ChunkEntry
    {
        Chunk chunk;
        ChunkState state;
    };

    // --- Lifecycle ---
    ChunkManager();
    ~ChunkManager();

    // --- Control ---
    // Called each frame on the main thread
    void update();
    // Consume meshed chunks from queue and change chunk state to Ready
    // Returns the ready meshes so the caller can upload them to the GPU
    std::vector<std::pair<glm::ivec3, ChunkMesh>> consumeReadyMeshes();

    // --- Chunk operations ---
    // void addChunk(Chunk chunk);
    // Changes chunk state to Requested
    void requestChunk(const glm::ivec3 &chunkPosition);
    const Chunk &getChunk(const glm::ivec3 &chunkPosition) const;
    bool hasChunk(const glm::ivec3 &chunkPosition) const;

private:
    // --- Private methods ---
    // Worker thread loop
    void workerLoop();

    // --- Private attributes, multithreading ---
    // HashMap so lookups by position are O(1) on average
    std::unordered_map<glm::ivec3, ChunkEntry, IVec3Hash> m_chunkDatabase;
    mutable std::mutex m_databaseMutex;
    // Chunks in state Requested
    std::queue<glm::ivec3> m_requestedQueue;
    std::mutex m_requestedQueueMutex;
    std::condition_variable m_requestCV;
    // Meshed chunks queue
    std::queue<std::pair<glm::ivec3, ChunkMesh>> m_meshedQueue;
    std::mutex m_meshedQueueMutex;
    // Thread control
    std::thread m_workerThread;
    std::atomic<bool> m_running{false};
};