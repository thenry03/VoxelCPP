#pragma once

#include "Chunk.hpp"
#include "ChunkNeighbourhood.hpp"

#include <glm/glm.hpp>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>

// --- Lifecycle States ---
// The 6 lifecycle states
enum class ChunkState : uint8_t
{
    Requested,  // Generation requested
    Generating, // Noise phase
    Generated,  // Ready to be meshed
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
        bool hasNotifiedReady = false;
    };

    // --- Lifecycle ---
    ChunkManager();
    ~ChunkManager();
    // Stops the worker thread; called before destruction to avoid teardown races
    void stop();

    // --- Control ---
    // Called each frame on the main thread
    void update(const glm::vec3 &playerPosition, int renderDistance);

    // --- Setup ---
    // Registers the callback fired when a chunk finishes generation
    void setOnChunkGenerated(std::function<void(const glm::ivec3 &)> callback);
    void setOnChunkReady(std::function<void(const glm::ivec3 &)> callback);
    void setOnChunkUnloaded(std::function<void(const glm::ivec3 &)> callback);

    // --- Chunk operations ---
    // Listed in the order a chunk travels through its lifecycle
    // Changes chunk state to Requested
    void requestChunk(const glm::ivec3 &chunkPosition);
    void markMeshing(const glm::ivec3 &chunkPosition);
    void markReady(const glm::ivec3 &chunkPosition);
    std::optional<ChunkNeighbourhood> snapshotNeighbourhood(const glm::ivec3 &chunkPosition) const;

    // --- Queries ---
    bool hasChunk(const glm::ivec3 &chunkPosition) const;

private:
    // --- Private methods ---
    // Unload chunks, load() is requestChunk() (infinite world generation)
    void unload(const glm::ivec3 &chunkPosition);
    // Worker thread loop
    void workerLoop();

    // --- Private attributes ---
    // Last chunk the player was in, to skip work when it does not change
    glm::ivec3 m_lastChunkPos = glm::ivec3(0, 0, 0);
    // Forces the first update to load the radius even at the origin
    bool m_firstUpdate = true;
    // Multithreading
    // HashMap so lookups by position are O(1) on average
    std::unordered_map<glm::ivec3, ChunkEntry, IVec3Hash> m_chunkDatabase;
    mutable std::mutex m_databaseMutex;
    // Chunks in state Requested
    std::queue<glm::ivec3> m_requestedQueue;
    std::mutex m_requestedQueueMutex;
    std::condition_variable m_requestCV;
    // Thread control
    std::thread m_workerThread;
    std::atomic<bool> m_running{false};
    std::function<void(const glm::ivec3 &)> m_onChunkGenerated;
    std::function<void(const glm::ivec3 &)> m_onChunkReady;
    std::function<void(const glm::ivec3 &)> m_onChunkUnloaded;
};