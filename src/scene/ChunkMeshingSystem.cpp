#include "ChunkMeshingSystem.hpp"

#include <optional>
#include <utility>

// ==========================================
// 1. LIFECYCLE
// ==========================================
ChunkMeshingSystem::ChunkMeshingSystem(ChunkManager &chunkManager)
    : m_chunkManager(chunkManager)
{
    // Set atomic flag to true
    m_running = true;
    // Run worker thread
    m_workerThread = std::thread(&ChunkMeshingSystem::workerLoop, this);
}

ChunkMeshingSystem::~ChunkMeshingSystem()
{
    stop();
}

void ChunkMeshingSystem::stop()
{
    // Idempotent: called explicitly from main and again from the destructor
    // 1. Set atomic flag to false
    // If not, worker thread might access non-existent variables
    m_running = false;
    // 2. Wake up worker
    // It might be blocked waiting for the condition
    // If not, the join below would freeze forever waiting for a blocked thread
    m_requestCV.notify_one();

    // 3. Wait for the thread to finish
    // After the first join joinable() is false, so a second stop() is a no-op
    if (m_workerThread.joinable())
        m_workerThread.join();
}

void ChunkMeshingSystem::enqueue(const glm::ivec3 &chunkPosition)
{
    {
        std::lock_guard<std::mutex> requestLock(m_requestedQueueMutex);
        // Push chunk position to requested queue for meshing
        m_requestedQueue.push(chunkPosition);
    }
    // There are chunks to be meshed
    m_requestCV.notify_one();
}

std::vector<std::pair<glm::ivec3, ChunkMesh>> ChunkMeshingSystem::consumeReadyMeshes()
{
    std::vector<std::pair<glm::ivec3, ChunkMesh>> ready;

    // 1. Empty queue to a local (move, not copy)
    {
        std::lock_guard<std::mutex> meshLock(m_meshedQueueMutex);
        while (!m_meshedQueue.empty())
        {
            // Push to the ready vector the front element of the queue
            ready.push_back(std::move(m_meshedQueue.front()));
            // Remove said element from the queue
            m_meshedQueue.pop();
        }
    }

    // 2. Set Ready state in the database
    for (auto &[position, mesh] : ready)
        m_chunkManager.markReady(position);

    return ready;
}

// ==========================================
// 3. PRIVATE METHODS
// ==========================================
void ChunkMeshingSystem::workerLoop()
{
    // Variables declared inside {} die after }
    while (m_running == true)
    {
        glm::ivec3 chunkPosition;

        // =========================
        // 1. EFFICIENT WAIT
        // =========================
        {
            std::unique_lock<std::mutex> requestLock(m_requestedQueueMutex);
            // Thread sleeps
            m_requestCV.wait(requestLock, [this]()
                             { return !m_requestedQueue.empty() || !m_running; });

            if (!m_running)
                break;

            // Safe fetching
            chunkPosition = m_requestedQueue.front();
            m_requestedQueue.pop();
        }

        // =========================
        // 2. MARK MESHING
        // =========================
        m_chunkManager.markMeshing(chunkPosition);

        // =========================
        // 3. MESHING
        // =========================
        // The chunk may have been unloaded; skip it if the snapshot is empty
        std::optional<ChunkNeighbourhood> chunkNeighbourhood =
            m_chunkManager.snapshotNeighbourhood(chunkPosition);
        if (!chunkNeighbourhood)
            continue;

        // Must survive until after delivery
        ChunkMesh chunkMesh = ChunkMesher::generateCulledMesh(*chunkNeighbourhood);

        // =========================
        // 4. DELIVERY
        // =========================
        {
            std::lock_guard<std::mutex> meshLock(m_meshedQueueMutex);
            // Access meshed queue to push a new meshed chunk
            m_meshedQueue.push({chunkPosition, chunkMesh});
        }
    }
}