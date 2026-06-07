#pragma once

#include <array>
#include <cstdint>

// --- Block texture layout data ---
// Holds the three texture IDs required to render any block type
struct BlockTextureLayout
{
    uint8_t top;    // Texture ID for the top face (+Y)
    uint8_t side;   // Texture ID for all side faces (North, South, East, West)
    uint8_t bottom; // Texture ID for the bottom face (-Y)
};

// --- Static map definition (magic) ---
// Fast O(1) lookup table indexed directly by Block ID
// Supports up to 256 unique block types
inline const std::array<BlockTextureLayout, 256> blockTextureMap = []()
{
    std::array<BlockTextureLayout, 256> map{};

    // Block 0: Air
    // Never rendered, safely defaults to 0
    map[0] = {0, 0, 0};

    // Block 1: Grass
    // Up: Grass (0) | Sides: Mixed (1) | Bottom: Dirt (2)
    map[1] = {0, 1, 2};

    // Block 2: Dirt
    // Up: Dirt (2) | Sides: Dirt (2) | Bottom: Dirt (2)
    map[2] = {2, 2, 2};

    // Block 3: Sand
    // Up: Sand (3) | Sides: Sand (3) | Bottom: Sand (3)
    map[3] = {3, 3, 3};

    // Block 4: Gravel
    // Up: Gravel (4) | Sides: Gravel (4) | Bottom: Gravel (4)
    map[4] = {4, 4, 4};

    // Block 5: Stone
    // Up: Stone (5) | Sides: Stone (5) | Bottom: Stone (5)
    map[5] = {5, 5, 5};

    // Block 6: Granite
    // Up: Granite (6) | Sides: Granite (6) | Bottom: Granite (6)
    map[6] = {6, 6, 6};

    // Block 7: Coal
    // Up: Coal (16) | Sides: Coal (16) | Bottom: Coal (16)
    map[7] = {16, 16, 16};

    // Block 8: Iron
    // Up: Iron (17) | Sides: Iron (17) | Bottom: Iron (17)
    map[8] = {17, 17, 17};

    // Block 9: Diamond
    // Up: Diamond (18) | Sides: Diamond (18) | Bottom: Diamond (18)
    map[9] = {18, 18, 18};

    // Block 10: Wood
    // Up: Rings (8) | Sides: Bark (9) | Bottom: Rings (8)
    map[10] = {8, 9, 8};

    // Block 11: Leaves
    // Up: Leaves (11) | Sides: Leaves (11) | Bottom: Leaves (11)
    map[11] = {11, 11, 11};

    // Block 12: Water
    // Up: Water (56) | Sides: Water (56) | Bottom: Water (56)
    map[12] = {56, 56, 56};

    // Block 13: Lava
    // Up: Lava (57) | Sides: Lava (57) | Bottom: Lava (57)
    map[13] = {57, 57, 57};

    // Return the fully initialized map
    return map;
}();