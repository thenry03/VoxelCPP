#include "ChunkManager.hpp"
#include "WorldGen.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>

namespace
{
    glm::ivec3 worldPosToChunkPos(const glm::vec3 &worldPosition)
    {
        // Chunk position:
        // x coord divide by width
        // z coord divide by depth
        return glm::ivec3(
            static_cast<int>(std::floor(worldPosition.x / static_cast<float>(Config::World::CHUNK_WIDTH))),
            0,
            static_cast<int>(std::floor(worldPosition.z / static_cast<float>(Config::World::CHUNK_DEPTH))));
    }

    // Player is in chunk (2, 0, 3) and renderDistance = 2
    // Loaded chunks: distance <= 2 in both x and z coords
    // x: [2-2, 2+2] = [0, 4]
    // z: [3-2, 3+2] = [1, 5]
    // Should (4, 0, 3) be loaded?
    // abs(4 - 2) = 2 <= 2: true
    // abs(3 - 3) = 0 <= 2: true
    // true && true = true: yes, inside radius
    bool isInsideRadius(const glm::ivec3 &chunkPosition,
                        const glm::ivec3 &playerChunkPosition,
                        int renderDistance)
    {
        return std::abs(chunkPosition.x - playerChunkPosition.x) <= renderDistance &&
               std::abs(chunkPosition.z - playerChunkPosition.z) <= renderDistance;
    }
}

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
void ChunkManager::update(const glm::vec3 &playerPosition, int renderDistance)
{
    glm::ivec3 currentChunkPosition = worldPosToChunkPos(playerPosition);

    // Only react when the player crosses into a new chunk
    // The first update always runs so the initial radius gets loaded
    if (!m_firstUpdate && currentChunkPosition == m_lastChunkPos)
        return;

    glm::ivec3 previousChunkPosition = m_lastChunkPos;
    bool isFirstUpdate = m_firstUpdate;
    m_firstUpdate = false;
    m_lastChunkPos = currentChunkPosition;

    // Request only the chunks that just entered the radius (the frontier)
    for (int x = -renderDistance; x <= renderDistance; x++)
        for (int z = -renderDistance; z <= renderDistance; z++)
        {
            glm::ivec3 candidatePosition = currentChunkPosition + glm::ivec3(x, 0, z);
            // Skip chunks that were already inside the radius last update
            // On the first update there is no previous radius, so request the whole disc
            if (!isFirstUpdate && isInsideRadius(candidatePosition, previousChunkPosition, renderDistance))
                continue;
            // requestChunk is idempotent, so no need to check existence first
            requestChunk(candidatePosition);
        }

    std::vector<glm::ivec3> toUnload;
    {
        std::lock_guard<std::mutex> dbLock(m_databaseMutex);
        for (auto &[position, entry] : m_chunkDatabase)
            if (!isInsideRadius(position, currentChunkPosition, renderDistance))
                toUnload.push_back(position);
    }

    for (const glm::ivec3 &position : toUnload)
        unload(position);
}

void ChunkManager::setOnChunkGenerated(std::function<void(const glm::ivec3 &)> callback)
{
    m_onChunkGenerated = std::move(callback);
}

void ChunkManager::setOnChunkReady(std::function<void(const glm::ivec3 &)> callback)
{
    m_onChunkReady = std::move(callback);
}

void ChunkManager::setOnChunkUnloaded(std::function<void(const glm::ivec3 &)> callback)
{
    m_onChunkUnloaded = std::move(callback);
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
    std::lock_guard<std::mutex> dbLock(m_databaseMutex);
    // The chunk may have been unloaded between generation and meshing
    auto it = m_chunkDatabase.find(chunkPosition);
    if (it != m_chunkDatabase.end())
        it->second.state = ChunkState::Meshing;
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
        // The chunk may have been unloaded before its mesh was consumed
        auto chunkIt = m_chunkDatabase.find(chunkPosition);
        if (chunkIt == m_chunkDatabase.end())
            return;
        auto &chunkEntry = chunkIt->second;
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

std::optional<ChunkNeighbourhood> ChunkManager::snapshotNeighbourhood(const glm::ivec3 &chunkPosition) const
{
    std::lock_guard<std::mutex> dbLock(m_databaseMutex);

    // The chunk may have been unloaded before the meshing worker reached it
    auto centerIt = m_chunkDatabase.find(chunkPosition);
    if (centerIt == m_chunkDatabase.end())
        return std::nullopt;

    ChunkNeighbourhood result{centerIt->second.chunk, {}};

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
        // No neighbour in this direction yet, skip before dereferencing
        if (it == m_chunkDatabase.end())
            continue;
        // A neighbour can be culled against as soon as its block data exists,
        // regardless of whether its own mesh is built yet
        ChunkState state = it->second.state;
        bool hasData = (state == ChunkState::Generated ||
                        state == ChunkState::Meshing ||
                        state == ChunkState::Ready);
        if (hasData)
            result.neighbours.emplace(neighbourPosition, it->second.chunk);
    }

    return result;
}

bool ChunkManager::hasChunk(const glm::ivec3 &chunkPosition) const
{
    std::lock_guard<std::mutex> dbLock(m_databaseMutex);
    return m_chunkDatabase.count(chunkPosition) > 0;
}

// ==========================================
// 3. PRIVATE METHODS
// ==========================================
void ChunkManager::unload(const glm::ivec3 &chunkPosition)
{
    {
        std::lock_guard<std::mutex> dbLock(m_databaseMutex);
        // Access database to remove unloaded chunk
        if (m_chunkDatabase.count(chunkPosition) > 0)
            m_chunkDatabase.erase(chunkPosition);
    }

    // Fire callback outside the lock
    if (m_onChunkUnloaded)
        m_onChunkUnloaded(chunkPosition);
}

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
            // The chunk may have been unloaded while queued; drop it if so
            auto it = m_chunkDatabase.find(chunkPosition);
            if (it == m_chunkDatabase.end())
                continue;
            // Change state to Generating
            it->second.state = ChunkState::Generating;
        }

        // Step 2
        // Chunk generation
        Chunk newChunk = WorldGen::generateChunk(chunkPosition, WorldGen::GenerationType::BLENDED_2D_NOISE);

        // Step 3
        {
            std::lock_guard<std::mutex> dbLock(m_databaseMutex);
            // The chunk may have been unloaded during generation; drop the result
            auto it = m_chunkDatabase.find(chunkPosition);
            if (it == m_chunkDatabase.end())
                continue;
            // Insert newly created chunk and change state to Generated
            it->second.chunk = std::move(newChunk);
            it->second.state = ChunkState::Generated;
        }

        // Step 4
        // Notify meshing system without knowing who it is
        if (m_onChunkGenerated)
            m_onChunkGenerated(chunkPosition);
    }
}