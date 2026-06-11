#include "Flora.hpp"
#include "TerrainShaper.hpp"
#include "Tree.hpp"

#include <cmath>
#include <cstdint>

// ==========================================
// 1. HELPER METHODS
// ==========================================
namespace
{
    // splitmix64: high quality integer mixer, deterministic, no floats
    uint64_t mix(uint64_t value)
    {
        value += 0x9E3779B97F4A7C15ull;
        value = (value ^ (value >> 30)) * 0xBF58476D1CE4E5B9ull;
        value = (value ^ (value >> 27)) * 0x94D049BB133111EBull;

        return value ^ (value >> 31);
    }

    // One independent stream per cell and channel (salt)
    uint64_t hashCell(int cellX, int cellZ, int seed, uint64_t salt)
    {
        uint64_t h = static_cast<uint32_t>(cellX);
        h = mix(h ^ (static_cast<uint64_t>(static_cast<uint32_t>(cellZ)) << 32));
        h = mix(h ^ static_cast<uint32_t>(seed));
        h = mix(h ^ salt);

        return h;
    }

    // Distinct salt per flora type keeps their hash streams independent;
    // trees consume salt+0/+1/+2, so leave a gap before the next type
    constexpr uint64_t TREE_SALT = 1000;

    // Cells that grow a tree at full density, out of 100
    constexpr float TREE_BASE_SPAWN = 50.0f;

    // Cell grid size in blocks; at most one candidate tree per cell
    constexpr int CELL_SIZE = 8;

    // True when a chunk-local position lies within this chunk's bounds
    bool isInsideChunk(int localX, int localZ, int y)
    {
        return localX >= 0 && localX < static_cast<int>(Config::World::CHUNK_WIDTH) &&
               localZ >= 0 && localZ < static_cast<int>(Config::World::CHUNK_DEPTH) &&
               y >= 0 && y < static_cast<int>(Config::World::CHUNK_HEIGHT);
    }

    void stampTree(Chunk &chunk,
                   const glm::ivec3 &chunkPosition,
                   int treeWorldX,
                   int surfaceY,
                   int treeWorldZ,
                   int trunkHeight)
    {
        // World offset of this chunk; cast the size to int so negative chunk
        // coordinates are not corrupted by unsigned arithmetic
        int chunkWorldX = chunkPosition.x * static_cast<int>(Config::World::CHUNK_WIDTH);
        int chunkWorldZ = chunkPosition.z * static_cast<int>(Config::World::CHUNK_DEPTH);

        // Trunk column in this chunk's local space; a tree may straddle chunks,
        // so blocks that fall outside are skipped and stamped by the owner chunk
        int localX = treeWorldX - chunkWorldX;
        int localZ = treeWorldZ - chunkWorldZ;

        // Trunk: stack of logs from just above the surface
        for (int y = surfaceY + 1; y <= surfaceY + trunkHeight; y++)
            if (isInsideChunk(localX, localZ, y))
                chunk.setBlock(localX, y, localZ, OAK_TREE.logBlockID);

        // Canopy: leaf layers wrapped around the top of the trunk
        int treeTop = surfaceY + trunkHeight;
        for (int layer = 0; layer < OAK_TREE.canopyHeight; layer++)
        {
            // Fill layers bottom-to-top, finishing level with the trunk top
            int y = treeTop - OAK_TREE.canopyHeight + 1 + layer;
            // Wide layers below, a narrow 3x3 cap on the very top
            int radius = (layer == OAK_TREE.canopyHeight - 1) ? 1 : OAK_TREE.canopyRadius;

            for (int dx = -radius; dx <= radius; dx++)
                for (int dz = -radius; dz <= radius; dz++)
                {
                    int leafX = localX + dx;
                    int leafZ = localZ + dz;
                    if (!isInsideChunk(leafX, leafZ, y))
                        continue;
                    // Only fill air, so leaves never carve trunk or terrain and
                    // overlapping canopies stay order-independent
                    if (chunk.getBlock(leafX, y, leafZ) == static_cast<uint8_t>(BlockType::Air))
                        chunk.setBlock(leafX, y, leafZ, OAK_TREE.leavesBlockID);
                }
        }
    }

    void stampTrees(Chunk &chunk,
                    const glm::ivec3 &chunkPosition,
                    const TerrainShaper &terrainShaper,
                    int seed,
                    uint64_t salt)
    {
        // World offset of this chunk; cast the size to int so negative chunk
        // coordinates are not corrupted by unsigned arithmetic
        int chunkWorldX = chunkPosition.x * static_cast<int>(Config::World::CHUNK_WIDTH);
        int chunkWorldZ = chunkPosition.z * static_cast<int>(Config::World::CHUNK_DEPTH);

        const float seaLevel = Config::World::SEA_LEVEL_FACTOR * Config::World::CHUNK_HEIGHT;
        const float treeLine = Config::World::TREE_LINE_FACTOR * Config::World::CHUNK_HEIGHT;
        const float fadeStart = Config::World::TREE_DENSITY_FADE_START_FACTOR * Config::World::CHUNK_HEIGHT;

        int halo = OAK_TREE.canopyRadius; // how far a tree can reach into a neighbour

        // Cells overlapping this chunk, expanded by the canopy halo so trees
        // rooted just outside still drop leaves in. floor (not integer /) so the
        // negative side rounds the correct way
        int cellMinX = static_cast<int>(std::floor((chunkWorldX - halo) / static_cast<float>(CELL_SIZE)));
        int cellMaxX = static_cast<int>(std::floor((chunkWorldX + static_cast<int>(Config::World::CHUNK_WIDTH) + halo) / static_cast<float>(CELL_SIZE)));
        int cellMinZ = static_cast<int>(std::floor((chunkWorldZ - halo) / static_cast<float>(CELL_SIZE)));
        int cellMaxZ = static_cast<int>(std::floor((chunkWorldZ + static_cast<int>(Config::World::CHUNK_DEPTH) + halo) / static_cast<float>(CELL_SIZE)));

        for (int cellX = cellMinX; cellX <= cellMaxX; cellX++)
            for (int cellZ = cellMinZ; cellZ <= cellMaxZ; cellZ++)
            {
                // One independent hash stream per decision (salt offset), so
                // position, height and spawn do not correlate with each other
                uint64_t posHash = hashCell(cellX, cellZ, seed, salt);
                uint64_t heightHash = hashCell(cellX, cellZ, seed, salt + 1);
                uint64_t spawnHash = hashCell(cellX, cellZ, seed, salt + 2);

                // Jitter the trunk to a block inside the cell; low and high bits
                // of one hash give two uncorrelated offsets
                int treeWorldX = cellX * CELL_SIZE + static_cast<int>(posHash % CELL_SIZE);
                int treeWorldZ = cellZ * CELL_SIZE + static_cast<int>((posHash >> 32) % CELL_SIZE);

                int surfaceY = terrainShaper.heightAt(static_cast<float>(treeWorldX),
                                                      static_cast<float>(treeWorldZ));

                // Plant only in the vegetation band: above sea level (lower
                // columns are sand) and below the tree line (higher columns are
                // bare rock). Pure test, so every chunk agrees which trees exist,
                // vital for halo trees whose trunk lives in a neighbouring chunk
                if (surfaceY < seaLevel || surfaceY > treeLine)
                    continue;

                // Density fades with altitude: full near the fade start, halved
                // at the tree line, so high slopes thin out before the bare peaks
                // (surfaceY is already inside [seaLevel, treeLine] here)
                float fadeT = (surfaceY - fadeStart) / (treeLine - fadeStart);
                float spawnChance = TREE_BASE_SPAWN * (1.0f - 0.5f * fadeT);
                if (static_cast<float>(spawnHash % 100) >= spawnChance)
                    continue;

                int trunkRange = OAK_TREE.trunkMaxHeight - OAK_TREE.trunkMinHeight + 1;
                int trunkHeight = OAK_TREE.trunkMinHeight + static_cast<int>(heightHash % trunkRange);

                stampTree(chunk, chunkPosition, treeWorldX, surfaceY, treeWorldZ, trunkHeight);
            }
    }
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
void Flora::stampFloraInChunk(Chunk &chunk,
                              const glm::ivec3 &chunkPosition,
                              const TerrainShaper &terrainShaper,
                              int seed)
{
    stampTrees(chunk, chunkPosition, terrainShaper, seed, TREE_SALT);
}
