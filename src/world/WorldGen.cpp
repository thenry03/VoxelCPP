#define FASTNOISE_IMPLEMENTATION

#include "WorldGen.hpp"
#include "Block.hpp"

#include <FastNoiseLite.h>

// ==========================================
// 1. HELPER METHODS
// ==========================================
namespace
{
    void generate2DNoise(Chunk &chunk)
    {
        // Define noise
        FastNoiseLite noise;

        // Configure the noise object
        // Simplex noise as a test
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        // No idea what the following three lines do, trying stuff
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        noise.SetFractalOctaves(Config::World::NOISE_OCTAVES);
        noise.SetFrequency(Config::World::NOISE_FREQUENCY);

        // Easter Egg hidden in noise seed
        noise.SetSeed(Config::World::NOISE_SEED);

        // Base height
        // With current value, if height is 256, 96 is the middle
        const float baseHeight = Config::World::TERRAIN_BASE * Config::World::CHUNK_HEIGHT;
        // Amplitude
        // 80 block range (up/down)
        const float amplitude = Config::World::TERRAIN_AMPLITUDE * Config::World::CHUNK_HEIGHT;

        // Chunk position does not change, out of the loop for efficiency
        glm::ivec3 chunkPosition = chunk.getPosition();

        // Generate the 2D heightmap
        for (int x = 0; x < static_cast<int>(Config::World::CHUNK_WIDTH); x++)
            for (int z = 0; z < static_cast<int>(Config::World::CHUNK_DEPTH); z++)
            {
                // World coords so the same seed doesn't produce the same heightmap on every chunk
                float worldX = static_cast<float>(chunkPosition.x * Config::World::CHUNK_WIDTH + x);
                float worldZ = static_cast<float>(chunkPosition.z * Config::World::CHUNK_DEPTH + z);

                // Sample 2D noise using horizontal X and Z coords
                float noiseValue = noise.GetNoise(static_cast<float>(worldX),
                                                  static_cast<float>(worldZ));

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
                    else if (y >= surfaceHeight - Config::World::TERRAIN_DIRT_DEPTH)
                        // Then some dirt
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                    else
                        // Then the rest is just stone
                        chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
                }
            }
    }

    // TODO
    void generate3DNoise([[maybe_unused]] Chunk &chunk)
    {

    }
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
// Public API implementation
Chunk WorldGen::generateChunk(const glm::ivec3 &chunkPosition, GenerationType type)
{
    // Create chunk object to return
    Chunk chunk(chunkPosition);

    // Route the execution based on choice
    switch (type)
    {
        // 2D noise
        case GenerationType::Simplex2D:
            generate2DNoise(chunk);
            break;
        // 3D noise
        case GenerationType::Simplex3D:
            generate3DNoise(chunk);
            break;
        default:
            generate2DNoise(chunk);
    }

    return chunk;
}