#pragma once

#include "../../renderer/ChunkMesher.hpp"
#include "../../world/ChunkManager.hpp"

#include <glm/glm.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ChunkMeshingSystem
{
public:
    // --- Lifecycle ---
    ChunkMeshingSystem(ChunkManager &chunkManager);
    ~ChunkMeshingSystem();
    // Stops the worker thread; called before destruction to avoid teardown races
    void stop();

    // --- Control ---
    void enqueue(const glm::ivec3 &chunkPosition);
    std::vector<std::pair<glm::ivec3, ChunkMesh>> consumeReadyMeshes();

private:
    // --- Private methods ---
    void workerLoop();

    // --- Private attributes, multithreading ---
    // Needed to snapshot neighbourhood and mark ready
    ChunkManager &m_chunkManager;
    // Chunks waiting to be meshed
    std::queue<glm::ivec3> m_requestedQueue;
    std::mutex m_requestedQueueMutex;
    std::condition_variable m_requestCV;
    // Meshed chunks waiting for GPU upload
    std::queue<std::pair<glm::ivec3, ChunkMesh>> m_meshedQueue;
    std::mutex m_meshedQueueMutex;
    // Thread control
    std::thread m_workerThread;
    std::atomic<bool> m_running{false};
};