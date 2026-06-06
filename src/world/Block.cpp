#include "Block.hpp"

// ==========================================
// 1. STATIC ARRAY DEFINITION
// ==========================================
BlockProperties BlockDatabase::m_database[static_cast<BlockDatabase::size_type>(BlockType::Count)];

// ==========================================
// 2. LIFECYCLE
// ==========================================
void BlockDatabase::init()
{
    // AIR
    // Air is non-solid, transparent and does not emit light
    m_database[static_cast<size_type>(BlockType::Air)] =
        {
            "Air",
            false, // isSolid
            true,  // isTransparent
            0,     // emittedLight
        };

    // GRASS
    // Grass is solid, non-transparent and does not emit light
    m_database[static_cast<size_type>(BlockType::Grass)] =
        {
            "Grass",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // DIRT
    // Dirt is solid, non-transparent and does not emit light
    m_database[static_cast<size_type>(BlockType::Dirt)] =
        {
            "Dirt",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // WATER
    // Water is liquid, transparent and does not emit light
    m_database[static_cast<size_type>(BlockType::Water)] =
        {
            "Water",
            false, // isSolid
            true,  // isTransparent
            0,     // emittedLight
        };

    // STONE
    // Stone is solid, non-transparent and does not emit light
    m_database[static_cast<size_type>(BlockType::Stone)] =
        {
            "Stone",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };
}