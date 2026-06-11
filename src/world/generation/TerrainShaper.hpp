#pragma once

#include <FastNoiseLite.h>

// --- Definition ---
// Pure surface-height function: configures the terrain noises once and resolves
// the blended height for any world column with no chunk dependency, so terrain
// fill and flora placement share a single source of truth
class TerrainShaper
{
public:
    explicit TerrainShaper(int seed);

    // Blended surface block height at a world column
    int heightAt(float worldX, float worldZ) const;

private:
    FastNoiseLite m_plainsNoise;
    FastNoiseLite m_mountainsNoise;
    FastNoiseLite m_riversNoise;
    FastNoiseLite m_selectorNoise;
    FastNoiseLite m_warpNoise;
};
