#include "ChunkManager.hpp"
#include "WorldGen.hpp"

#include <array>
#include <utility>

// ==========================================
// 1. LIFECYCLE
// ==========================================
ChunkManager::ChunkManager()
{
    // Set atomic flag to true
    m_running = true;
    // Run worker thread
    m_workerThread = std::thread(&ChunkManager::workerLoop, this);
}

ChunkManager::~ChunkManager()
{
    // 1. Set atomic flag to false
    // If not, worker thread might access non-existent variables
    m_running = false;
    // 2. Wake up worker
    // It might be blocked waiting for the condition
    // If not, the destructor method will freeze forever,
    // waiting for a blocked thread
    m_requestCV.notify_one();

    // Wait for the thread to finish
    if (m_workerThread.joinable())
        m_workerThread.join();
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void ChunkManager::requestChunk(const glm::ivec3 &chunkPosition)
{
    // Grab both locks at once, deadlock-free
    {
        std::scoped_lock lock(m_databaseMutex, m_requestedQueueMutex);
        // 1. Create entry with Requested State
        if (m_chunkDatabase.count(chunkPosition) > 0)
            return;
        m_chunkDatabase.emplace(chunkPosition,
                                ChunkEntry{
                                    Chunk(chunkPosition),
                                    ChunkState::Requested});

        // 2. Queue the position
        m_requestedQueue.push(chunkPosition);
    }
    // Notify after lock release to prevent "hurry and wait" situations
    m_requestCV.notify_one();
}

std::vector<std::pair<glm::ivec3, ChunkMesh>> ChunkManager::consumeReadyMeshes()
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
    {
        std::lock_guard<std::mutex> dbLock(m_databaseMutex);
        for (const auto &[position, mesh] : ready)
        {
            auto it = m_chunkDatabase.find(position);
            // Set to Ready if it exists and is in Meshing state
            if (it != m_chunkDatabase.end() && it->second.state == ChunkState::Meshing)
                it->second.state = ChunkState::Ready;
        }
    }

    return ready;
}

const Chunk &ChunkManager::getChunk(const glm::ivec3 &position) const
{
    return m_chunkDatabase.at(position).chunk;
}

bool ChunkManager::hasChunk(const glm::ivec3 &position) const
{
    return m_chunkDatabase.count(position) > 0;
}

// ==========================================
// 3. PRIVATE METHODS
// ==========================================
void ChunkManager::workerLoop()
{
    // Variables declared inside {} die after }
    while (m_running == true)
    {
        glm::ivec3 chunkPosition;

        // =========================
        // 1. EFFICIENT WAIT
        // =========================
        {
            // Access Requested state queue
            std::unique_lock<std::mutex> requestLock(m_requestedQueueMutex);
            // Thread sleeps
            // Only wakes up if a chunk is added or if program closes
            m_requestCV.wait(requestLock, [this]()
                             { return !m_requestedQueue.empty() || !m_running; });

            if (!m_running)
                break; // Wake up if needed

            // Now, fetching the chunk position is safe
            chunkPosition = m_requestedQueue.front();
            m_requestedQueue.pop();
        }

        // =========================
        // 2. GENERATION
        // =========================
        // Step 1
        {
            std::lock_guard<std::mutex> dbLock(m_databaseMutex);
            // Access database
            auto &chunkEntry = m_chunkDatabase.at(chunkPosition);
            // Change state to Generating
            chunkEntry.state = ChunkState::Generating;
        }

        // Step 2
        // Chunk generation
        Chunk newChunk = WorldGen::generateChunk(chunkPosition, WorldGen::GenerationType::Simplex2D);

        // Step 3
        {
            std::lock_guard<std::mutex> dbLock(m_databaseMutex);
            // Access database again to insert newly created chunk and change state to Meshing
            m_chunkDatabase.at(chunkPosition).chunk = std::move(newChunk);
            m_chunkDatabase.at(chunkPosition).state = ChunkState::Meshing;
        }

        // =========================
        // 3. MESHING
        // =========================
        // Must survive until after delivery
        ChunkMesh chunkMesh;

        // Step 1
        ChunkNeighbourhood snapshot = [this, &chunkPosition]() -> ChunkNeighbourhood
        {
            std::lock_guard<std::mutex> dbLock(m_databaseMutex);

            // Access database yet again
            ChunkNeighbourhood result{m_chunkDatabase.at(chunkPosition).chunk, {}};

            // 6 possible directions
            static const std::array<glm::ivec3, 6> directions = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};

            // Iterate through directions and add neighbours
            for (const auto &dir : directions)
            {
                glm::ivec3 neighbourPos = chunkPosition + dir;
                auto it = m_chunkDatabase.find(neighbourPos);
                // If neighbour exists in investigated direction and it is Ready, add it
                if (it != m_chunkDatabase.end() && it->second.state == ChunkState::Ready)
                    result.neighbours.emplace(neighbourPos, it->second.chunk);

                // TODO (technical debt): chunks meshed before a neighbour exists keep
                // their boundary faces drawn, leaving hidden geometry at the seam
                // Fix: when a chunk turns Ready (in update()), re-enqueue its existing
                // neighbours for meshing so they recompute their borders
                // Cheaper still: gate meshing on neighbours being generated, not Ready
            }

            return result;
        }();

        // Step 2
        // Mesh AFTER unlocking (much more efficient)
        chunkMesh = ChunkMesher::generateCulledMesh(snapshot);

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