#pragma once

#include "../core/Constants.hpp"

#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

class Chunk
{
public:
    // --- Lifecycle ---
    Chunk(const glm::ivec3 &position);
    ~Chunk() = default;

    // --- Queries ---
    uint8_t getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, uint8_t blockID);
    glm::ivec3 getPosition() const
    {
        return m_chunkPosition;
    }

private:
    // Transform local 3D coordinates in 1D array index
    // Example: accessing block (x, y, z) = (3, 5, 2)
    // - First: + y * 16 * 16 = 5 * 16 * 16 = 1280 ignored blocks
    //   Skips 5 full chunk "floors" (Y coordinate)
    //   Each floor is a 16 * 16 grid
    // - Second: + z * 16 = 2 * 16 = 32 ignored blocks
    //   Skips 2 rows of blocks within that same floor (Z coordinate)
    // - Third: + x = 3 ignored blocks (X coordinate)
    //   Skips 3 individual blocks within that same row of that same floor
    //   Is now pointing at the fourth block of the third row of the sixth floor
    //   Indexes: (4-1, 6-1, 3-1) = (3, 5, 2)
    inline std::size_t getIndex(int x, int y, int z) const
    {
        return static_cast<std::size_t>(x +
                                        (z * Config::World::CHUNK_WIDTH) +
                                         (y *
                                          (Config::World::CHUNK_WIDTH *
                                           Config::World::CHUNK_DEPTH
                                          )
                                         )
                                       );
    }
    glm::ivec3 m_chunkPosition;
    // 1D array of blocks
    // Stored in HEAP
    std::vector<uint8_t> m_blockIDs;
};