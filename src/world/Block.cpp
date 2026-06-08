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
    m_database[static_cast<uint8_t>(BlockType::Air)] =
        {
            "Air",
            false, // isSolid
            true,  // isTransparent
            0,     // emittedLight
        };

    // GRASS
    // Grass is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Grass)] =
        {
            "Grass",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // DIRT
    // Dirt is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Dirt)] =
        {
            "Dirt",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // SAND
    // Sand is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Sand)] =
        {
            "Sand",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // GRAVEL
    // Gravel is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Gravel)] =
        {
            "Gravel",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // STONE
    // Stone is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Stone)] =
        {
            "Stone",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // GRANITE
    // Granite is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Granite)] =
        {
            "Granite",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // COAL
    // Coal is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Coal)] =
        {
            "Coal",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // IRON
    // Iron is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Iron)] =
        {
            "Iron",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // DIAMOND
    // Diamond is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Diamond)] =
        {
            "Diamond",
            true,  // isSolid
            false, // isTransparent
            5,     // emittedLight
        };

    // WOOD
    // Wood is solid, non-transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Wood)] =
        {
            "Wood",
            true,  // isSolid
            false, // isTransparent
            0,     // emittedLight
        };

    // LEAVES
    // Leaves are solid, transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Leaves)] =
        {
            "Leaves",
            true, // isSolid
            true, // isTransparent
            0,    // emittedLight
        };

    // WATER
    // Water is liquid, transparent and does not emit light
    m_database[static_cast<uint8_t>(BlockType::Water)] =
        {
            "Water",
            false, // isSolid
            true,  // isTransparent
            0,     // emittedLight
        };

    // LAVA
    // Lava is liquid, non-transparent and emits light
    m_database[static_cast<uint8_t>(BlockType::Lava)] =
        {
            "Lava",
            false, // isSolid
            false, // isTransparent
            15,    // emittedLight
        };
}