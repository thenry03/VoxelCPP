#include "TerrainShaper.hpp"

#include "../../core/Constants.hpp"

#include <glm/glm.hpp>

#include <cmath>

// ==========================================
// 1. HELPER METHODS
// ==========================================
namespace
{
    float lerp(float a, float b, float t)
    {
        return (a + (b - a) * t);
    }
}

// ==========================================
// 2. LIFECYCLE
// ==========================================
TerrainShaper::TerrainShaper(int seed)
{
    // Configure each noise
    // Plains
    m_plainsNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_plainsNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_plainsNoise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
    m_plainsNoise.SetFrequency(Config::World::PLAINS_NOISE_FREQUENCY);
    m_plainsNoise.SetSeed(seed);

    // Mountains
    m_mountainsNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_mountainsNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_mountainsNoise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
    m_mountainsNoise.SetFrequency(Config::World::MOUNTAINS_NOISE_FREQUENCY);
    m_mountainsNoise.SetSeed(seed + 1);

    // Rivers
    m_riversNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_riversNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_riversNoise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
    m_riversNoise.SetFrequency(Config::World::RIVERS_NOISE_FREQUENCY);
    m_riversNoise.SetSeed(seed + 2);

    // Selector
    m_selectorNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_selectorNoise.SetFractalType(FastNoiseLite::FractalType_None);
    m_selectorNoise.SetFrequency(Config::World::SELECTOR_NOISE_FREQUENCY);
    m_selectorNoise.SetSeed(seed + 3);

    // Warp
    m_warpNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_warpNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_warpNoise.SetFractalOctaves(Config::World::WARP_NOISE_OCTAVES);
    m_warpNoise.SetFrequency(Config::World::WARP_NOISE_FREQUENCY);
    m_warpNoise.SetSeed(seed + 4);
}

// ==========================================
// 3. PUBLIC METHODS
// ==========================================
int TerrainShaper::heightAt(float worldX, float worldZ) const
{
    // Base height and amplitudes
    const float baseHeight = Config::World::TERRAIN_BASE * Config::World::CHUNK_HEIGHT;
    const float seaLevel = Config::World::SEA_LEVEL_FACTOR * Config::World::CHUNK_HEIGHT;
    const float plainsAmplitude = Config::World::PLAINS_TERRAIN_AMPLITUDE *
                                  Config::World::CHUNK_HEIGHT;
    const float mountainsAmplitude = Config::World::MOUNTAINS_TERRAIN_AMPLITUDE *
                                     Config::World::CHUNK_HEIGHT;

    // =========================
    // 1. Domain Warping
    // =========================
    // Horizontal force
    float warpX = m_warpNoise.GetNoise(worldX, worldZ) * 40.0f;
    float warpZ = m_warpNoise.GetNoise(worldX + 500.0f, worldZ + 500.0f) * 40.f;

    // Use deformed coordinates
    float warpedX = worldX + warpX;
    float warpedZ = worldZ + warpZ;

    // =========================
    // 2. Continental Map
    // =========================
    // Range [-1.0, 1.0]
    // Bias upward so land dominates over sea
    float continentality = m_selectorNoise.GetNoise(warpedX, warpedZ) * 0.8f + 0.25f;

    // =========================
    // 3. Height Layers
    // =========================
    // Deep sea area
    float seaDepth = (continentality + 0.05f) * (seaLevel * 0.70f);
    // Gentle relief so the ocean floor is not a smooth bowl
    float floorDetail = m_plainsNoise.GetNoise(warpedX * 2.0f, warpedZ * 2.0f) * 6.0f;
    float seaHeight = baseHeight + seaDepth + floorDetail;

    // Coast area
    // Create beaches by creating a smooth transition
    float plainsRaw = m_plainsNoise.GetNoise(warpedX, warpedZ) * plainsAmplitude;
    float coastFactor = glm::smoothstep(-0.05f, 0.2f, continentality);
    float plainsHeight = baseHeight + (plainsRaw * coastFactor);

    // Mountains: broad massifs tapering to a rounded summit
    // Normalize the FBm to [0, 1], then a power curve keeps the
    // base wide and lets only the tallest masses reach a peak
    float mountainsRaw = (m_mountainsNoise.GetNoise(warpedX, warpedZ) + 1.0f) * 0.5f;
    float massif = std::pow(mountainsRaw, 3.0f);
    float mountainHeight = baseHeight + massif * mountainsAmplitude * 1.5f;

    // Rivers between hills
    // River mask
    float riverMask = 1.0f - std::fabs(m_riversNoise.GetNoise(warpedX, warpedZ));
    riverMask = std::pow(riverMask, 4.0f); // Lower exponent = wider channel
    // Carve down to a bed below sea level so the global fill floods it
    float riverBed = seaLevel - 3.0f;

    // =========================
    // 4. Blending
    // =========================
    float finalHeight;

    if (continentality < -0.05f)
    {
        // Smooth transition from deep sea to coastline
        float coastWeight = glm::smoothstep(-0.5f, 0.0f, continentality);
        finalHeight = lerp(seaHeight, plainsHeight, coastWeight);
    }
    else
    {
        // Smooth transition between plains and mountains
        float mountainWeight = glm::smoothstep(0.0f, 0.5f, continentality);
        finalHeight = lerp(plainsHeight, mountainHeight, mountainWeight);
    }

    // =========================
    // 5. River carving
    // =========================
    // Cut the channel into whatever terrain the blend produced;
    // the bed sits below sea level so the global water fill floods it
    finalHeight = lerp(finalHeight, riverBed, riverMask);

    return glm::clamp(static_cast<int>(finalHeight),
                      0,
                      static_cast<int>(Config::World::CHUNK_HEIGHT) - 1);
}
