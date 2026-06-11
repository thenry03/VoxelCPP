#define FASTNOISE_IMPLEMENTATION

#include "WorldGen.hpp"
#include "Block.hpp"

#include <FastNoiseLite.h>

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

    void generateSingle2DNoise(Chunk &chunk)
    {
        // Define noise
        FastNoiseLite noise;

        // Configure the noise object
        // Simplex noise as a test
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        // Fractal Brownian Motion: layers multiple noise octaves
        // It does so at increasing frequencies and decreasing amplitudes
        // Produces natural-looking terrain
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        noise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
        noise.SetFrequency(Config::World::MOUNTAINS_NOISE_FREQUENCY);

        // Easter Egg hidden in noise seed
        noise.SetSeed(Config::World::WORLD_SEED);

        // Mid-level of the terrain, as a fraction of the chunk height
        const float baseHeight = Config::World::TERRAIN_BASE * Config::World::CHUNK_HEIGHT;
        // How far the surface rises and falls around the base height
        const float amplitude = Config::World::MOUNTAINS_TERRAIN_AMPLITUDE * Config::World::CHUNK_HEIGHT;

        // Chunk position does not change, out of the loop for efficiency
        glm::ivec3 chunkPosition = chunk.getPosition();

        // Generate the 2D heightmap
        for (int x = 0; x < static_cast<int>(Config::World::CHUNK_WIDTH); x++)
            for (int z = 0; z < static_cast<int>(Config::World::CHUNK_DEPTH); z++)
            {
                // World coords so the same seed doesn't produce the same heightmap on every chunk
                float worldX = static_cast<float>(chunkPosition.x * static_cast<int>(Config::World::CHUNK_WIDTH) + x);
                float worldZ = static_cast<float>(chunkPosition.z * static_cast<int>(Config::World::CHUNK_DEPTH) + z);

                // Sample 2D noise using horizontal X and Z coords
                float noiseValue = noise.GetNoise(worldX, worldZ);

                // Map the noise value from [-1.0, 1.0] to usable height block index
                int surfaceHeight = static_cast<int>((noiseValue * amplitude) + baseHeight);

                // NOW loop through y
                for (int y = 0; y < static_cast<int>(Config::World::CHUNK_HEIGHT); y++)
                {
                    if (y > surfaceHeight)
                        // Anything above ground is air
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                    else if (y == surfaceHeight)
                        // Exactly at surface height is grass
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Grass));
                    else if (y >= surfaceHeight - Config::World::DIRT_DEPTH)
                        // Then some dirt
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                    else
                        // Then the rest is just stone
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                }
            }
    }

    void generateBlended2DNoise(Chunk &chunk)
    {
        // Four independent noise sources
        FastNoiseLite plainsNoise;
        FastNoiseLite mountainsNoise;
        FastNoiseLite riversNoise;
        FastNoiseLite selectorNoise;
        FastNoiseLite warpNoise;

        // Configure each noise
        // Plains
        plainsNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        plainsNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        plainsNoise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
        plainsNoise.SetFrequency(Config::World::PLAINS_NOISE_FREQUENCY);
        plainsNoise.SetSeed(Config::World::WORLD_SEED);

        // Mountains
        mountainsNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        mountainsNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        mountainsNoise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
        mountainsNoise.SetFrequency(Config::World::MOUNTAINS_NOISE_FREQUENCY);
        mountainsNoise.SetSeed(Config::World::WORLD_SEED + 1);

        // Rivers
        riversNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        riversNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        riversNoise.SetFractalOctaves(Config::World::WORLD_NOISE_OCTAVES);
        riversNoise.SetFrequency(Config::World::RIVERS_NOISE_FREQUENCY);
        riversNoise.SetSeed(Config::World::WORLD_SEED + 2);

        // Selector
        selectorNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        selectorNoise.SetFractalType(FastNoiseLite::FractalType_None);
        selectorNoise.SetFrequency(Config::World::SELECTOR_NOISE_FREQUENCY);
        selectorNoise.SetSeed(Config::World::WORLD_SEED + 3);

        // Warp
        warpNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        warpNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        warpNoise.SetFractalOctaves(Config::World::WARP_NOISE_OCTAVES);
        warpNoise.SetFrequency(Config::World::WARP_NOISE_FREQUENCY);
        warpNoise.SetSeed(Config::World::WORLD_SEED + 4);

        // Base height and amplitudes
        const float baseHeight = Config::World::TERRAIN_BASE * Config::World::CHUNK_HEIGHT;
        const float seaLevel = Config::World::SEA_LEVEL_FACTOR * Config::World::CHUNK_HEIGHT;
        const float plainsAmplitude = Config::World::PLAINS_TERRAIN_AMPLITUDE *
                                      Config::World::CHUNK_HEIGHT;
        const float mountainsAmplitude = Config::World::MOUNTAINS_TERRAIN_AMPLITUDE *
                                         Config::World::CHUNK_HEIGHT;

        glm::ivec3 chunkPosition = chunk.getPosition();

        // Build the blended 2D heightmap
        for (int x = 0; x < static_cast<int>(Config::World::CHUNK_WIDTH); x++)
            for (int z = 0; z < static_cast<int>(Config::World::CHUNK_DEPTH); z++)
            {
                // World coordinates so the heightmap is continuous across chunks
                float worldX = static_cast<float>(
                    chunkPosition.x *
                        static_cast<int>(Config::World::CHUNK_WIDTH) +
                    x);
                float worldZ = static_cast<float>(
                    chunkPosition.z *
                        static_cast<int>(Config::World::CHUNK_DEPTH) +
                    z);

                // =========================
                // 1. Domain Warping
                // =========================
                // Horizontal force
                float warpX = warpNoise.GetNoise(worldX, worldZ) * 40.0f;
                float warpZ = warpNoise.GetNoise(worldX + 500.0f, worldZ + 500.0f) * 40.f;

                // Use deformed coordinates
                float warpedX = worldX + warpX;
                float warpedZ = worldZ + warpZ;

                // =========================
                // 2. Continental Map
                // =========================
                // Range [-1.0, 1.0]
                // Bias upward so land dominates over sea
                float continentality = selectorNoise.GetNoise(warpedX, warpedZ) * 0.8f + 0.25f;

                // =========================
                // 3. Height Layers
                // =========================
                // Deep sea area
                float seaDepth = (continentality + 0.05f) * (seaLevel * 0.70f);
                // Gentle relief so the ocean floor is not a smooth bowl
                float floorDetail = plainsNoise.GetNoise(warpedX * 2.0f, warpedZ * 2.0f) * 6.0f;
                float seaHeight = baseHeight + seaDepth + floorDetail;

                // Coast area
                // Create beaches by creating a smooth transition
                float plainsRaw = plainsNoise.GetNoise(warpedX, warpedZ) * plainsAmplitude;
                float coastFactor = glm::smoothstep(-0.05f, 0.2f, continentality);
                float plainsHeight = baseHeight + (plainsRaw * coastFactor);

                // Mountains: broad massifs tapering to a rounded summit
                // Normalize the FBm to [0, 1], then a power curve keeps the
                // base wide and lets only the tallest masses reach a peak
                float mountainsRaw = (mountainsNoise.GetNoise(warpedX, warpedZ) + 1.0f) * 0.5f;
                float massif = std::pow(mountainsRaw, 3.0f);
                float mountainHeight = baseHeight + massif * mountainsAmplitude * 1.5f;

                // Rivers between hills
                // River mask
                float riverMask = 1.0f - std::fabs(riversNoise.GetNoise(warpedX, warpedZ));
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

                int surfaceHeight = glm::clamp(static_cast<int>(finalHeight),
                                               0,
                                               static_cast<int>(Config::World::CHUNK_HEIGHT) - 1);

                bool underwater = surfaceHeight < seaLevel;

                // Fill the column from the surface down
                for (int y = 0; y < static_cast<int>(Config::World::CHUNK_HEIGHT); y++)
                {
                    if (y > surfaceHeight)
                        // Above ground: water up to sea level, air above
                        if (y <= seaLevel)
                            chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Water));
                        else
                            chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Air));
                    else if (y == surfaceHeight)
                        // Exactly at surface height is grass (or sand if underwater)
                        if (underwater)
                            chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Sand));
                        else
                            chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Grass));
                    else if (y >= surfaceHeight - Config::World::DIRT_DEPTH)
                        // Subsurface layers: sand under water, dirt otherwise
                        if (underwater)
                            chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Sand));
                        else
                            chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                    else
                        // Then the rest is just stone
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                }
            }
    }

    // TODO: implement 3D cave and overhang generation using Simple3D noise
    void generate3DNoise([[maybe_unused]] Chunk &chunk)
    {
    }
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
Chunk WorldGen::generateChunk(const glm::ivec3 &chunkPosition, GenerationType type)
{
    // Create chunk object to return
    Chunk chunk(chunkPosition);

    // Route the execution based on choice
    switch (type)
    {
    // 2D noise
    case GenerationType::SINGLE_2D_NOISE:
        generateSingle2DNoise(chunk);
        break;
    case GenerationType::BLENDED_2D_NOISE:
        generateBlended2DNoise(chunk);
        break;
    // 3D noise
    case GenerationType::TODO:
        generate3DNoise(chunk);
        break;
    default:
        generateBlended2DNoise(chunk);
    }

    return chunk;
}