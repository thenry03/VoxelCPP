#pragma once

#include <cstdint>
#include <string>

// --- Block type definition ---
// Defines block types
// Uses 'uint8_t' as underlying type
// Limits block types to 256, WAY more than enough
enum class BlockType : uint8_t
{
    Air = 0,

    // Surface
    Grass = 1,
    Dirt = 2,
    Sand = 3,
    Gravel = 4,

    // Underground
    Stone = 5,
    Granite = 6,
    Coal = 7,
    Iron = 8,
    Diamond = 9,

    // Flora
    Wood = 10,
    Leaves = 11,

    // Liquids
    Water = 12,
    Lava = 13,

    Count // How many blocks are there?
};

// --- Block static properties ---
// This struct acts as the technical information of each block
// Is not stored inside a Chunk type
// No texture IDs, that is a renderer responsability
struct BlockProperties
{
    std::string_view name;
    bool isSolid;
    bool isTransparent;
    uint8_t emittedLight;
};

// --- Block database, global catalogue ---
// This class centralizes information
// Any system may consult a certain block's properties anytime
class BlockDatabase
{
public:
    // --- Lifecycle ---
    static void init();

    // --- Queries ---
    // Returns block properties passing enum type
    // Example: BlockDatabase::get(BlockType::Stone)
    // Returns reference (&) so as to not copy the struct in memory
    static const BlockProperties& get(BlockType type)
    {
        // Cast Enum to its numeric value to use as array index
        return m_database[static_cast<size_type>(type)];
    }

    // Does the same, but passing ID as a byte
    static const BlockProperties& get(uint8_t id)
    {
        return m_database[id];
    }

private:
    // Defines the integer type the enum uses
    using size_type = std::underlying_type_t<BlockType>;
    // Physical array in RAM where properties are stored
    // Its size is defined during compilation (Count)
    static BlockProperties m_database[static_cast<size_type>(BlockType::Count)];
};