#pragma once

#include "../blocks/Block.hpp"

#include <cstdint>

// --- Tree data ---
struct Tree
{
    uint8_t logBlockID;
    uint8_t leavesBlockID;
    int trunkMinHeight;
    int trunkMaxHeight;
    int canopyRadius;
    int canopyHeight;
};

inline constexpr Tree OAK_TREE = {
    static_cast<uint8_t>(BlockType::Wood),
    static_cast<uint8_t>(BlockType::Leaves),
    4, 6, // trunk height range
    2,    // canopy radius (5x5 base)
    3     // canopy layers
};

enum class TreeSpecies : uint8_t
{
    Oak,
    Count
};