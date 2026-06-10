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
    stop();
}

void ChunkManager::stop()
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

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void ChunkManager::setOnChunkGenerated(std::function<void(const glm::ivec3 &)> callback)
{
    m_onChunkGenerated = std::move(callback);
}

void ChunkManager::setOnChunkReady(std::function<void(const glm::ivec3 &)> callback)
{
    m_onChunkReady = std::move(callback);
}

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

void ChunkManager::markMeshing(const glm::ivec3 &chunkPosition)
{
    {
        std::lock_guard<std::mutex> dbLock(m_databaseMutex);
        // Access database and mark chunk as Meshing
        m_chunkDatabase.at(chunkPosition).state = ChunkState::Meshing;
    }
}

void ChunkManager::markReady(const glm::ivec3 &chunkPosition)
{
    // 6 possible directions
    static const std::array<glm::ivec3, 6> directions = {{{1, 0, 0},
                                                          {-1, 0, 0},
                                                          {0, 1, 0},
                                                          {0, -1, 0},
                                                          {0, 0, 1},
                                                          {0, 0, -1}}};

    // Collect neighbours to re-enqueue outside the lock
    std::vector<glm::ivec3> toRemesh;
    {
        std::lock_guard<std::mutex> dbLock(m_databaseMutex);
        // Acess database and set state to Ready
        auto &chunkEntry = m_chunkDatabase.at(chunkPosition);
        chunkEntry.state = ChunkState::Ready;

        // Re-mesh neighbours only the first time this chunk becomes ready
        if (!chunkEntry.hasNotifiedReady)
        {
            chunkEntry.hasNotifiedReady = true;
            // Iterate through all directions again to cull more faces
            for (const auto &dir : directions)
            {
                glm::ivec3 neighbourPosition = chunkPosition + dir;
                auto it = m_chunkDatabase.find(neighbourPosition);
                if (it != m_chunkDatabase.end() && it->second.state == ChunkState::Ready)
                    toRemesh.push_back(neighbourPosition);
            }
        }
    }

    // Fire callbacks outside the lock
    if (m_onChunkReady)
        for (const auto &pos : toRemesh)
            m_onChunkReady(pos);
}

ChunkNeighbourhood ChunkManager::snapshotNeighbourhood(const glm::ivec3 &chunkPosition) const
{
    ChunkNeighbourhood snapshot = [this, chunkPosition]() -> ChunkNeighbourhood
    {
        std::lock_guard<std::mutex> dbLock(m_databaseMutex);

        // Access database yet again
        ChunkNeighbourhood result{m_chunkDatabase.at(chunkPosition).chunk, {}};

        // 6 possible directions
        static const std::array<glm::ivec3, 6> directions = {{{1, 0, 0},
                                                              {-1, 0, 0},
                                                              {0, 1, 0},
                                                              {0, -1, 0},
                                                              {0, 0, 1},
                                                              {0, 0, -1}}};

        // Iterate through directions and add neighbours
        for (const auto &dir : directions)
        {
            glm::ivec3 neighbourPosition = chunkPosition + dir;
            auto it = m_chunkDatabase.find(neighbourPosition);
            // If neighbour exists in investigated direction and is Ready, add it
            if (it != m_chunkDatabase.end() && it->second.state == ChunkState::Ready)
                result.neighbours.emplace(neighbourPosition, it->second.chunk);
        }

        return result;
    }();

    return snapshot;
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
            // Access database again to insert newly created chunk and change state to Generated
            m_chunkDatabase.at(chunkPosition).chunk = std::move(newChunk);
            m_chunkDatabase.at(chunkPosition).state = ChunkState::Generated;
        }

        // Step 4
        // Notify meshing system without knowing who it is
        if (m_onChunkGenerated)
            m_onChunkGenerated(chunkPosition);
    }
}