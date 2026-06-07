#include "../world/Block.hpp"
#include "ChunkMesher.hpp"

#include <array>

// ==========================================
// 1. STRUCT DEFINITION
// ==========================================
// Each block has 6 faces
// Each face has different offsets and normal (neighbout direction)
struct Face
{
    std::array<glm::vec3, 4> offsets; // Offsets for each face in CCW order
    glm::vec3 normal;                 // Normal vector
    glm::ivec3 neighbourOffset;       // The neighbor's offset matches the integer normal
};

// ==========================================
// 2. STATIC ARRAY DEFINITION
// ==========================================
const std::array<Face, 6> cubeFaces = {{
    // North (-Z)
    {
        // Offsets
        {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)},
        // Normal and neighbour
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::ivec3(0, 0, -1)},
    // South (+Z)
    {
        // Offsets
        {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 1.0f)},
        // Normal and neighbour
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::ivec3(0, 0, 1)},
    // East (+X)
    {
        {glm::vec3(1.0f, 0.0f, 0.0f),
         glm::vec3(1.0f, 1.0f, 0.0f),
         glm::vec3(1.0f, 1.0f, 1.0f),
         glm::vec3(1.0f, 0.0f, 1.0f)},
        // Normal and neighbour
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::ivec3(1, 0, 0)},
    // West (-X)
    {
        {glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(0.0f, 0.0f, 1.0f),
         glm::vec3(0.0f, 1.0f, 1.0f),
         glm::vec3(0.0f, 1.0f, 0.0f)},
        // Normal and neighbour
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::ivec3(-1, 0, 0)},
    // Top (+Y)
    {
        // Offsets
        {
            glm::vec3(0.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)},
        // Normal and neighbour
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::ivec3(0, 1, 0)},
    // Bottom (-Y)
    {
        // Offsets
        {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 1.0f)},
        // Normal and neighbour
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::ivec3(0, -1, 0)},
}};

// ==========================================
// 3. PUBLIC METHODS
// ==========================================
// Dumb mesh (draws everything)
ChunkMesh ChunkMesher::generateDumbMesh(const Chunk &chunk)
{
    ChunkMesh dumbMesh;
    // Iterate over each voxel
    // - Generate one 6-sided cube per solid voxel
    // - Skip non-solid voxels
    for (int y = 0; y < static_cast<int>(Config::World::CHUNK_HEIGHT); y++)
        for (int z = 0; z < static_cast<int>(Config::World::CHUNK_DEPTH); z++)
            for (int x = 0; x < static_cast<int>(Config::World::CHUNK_WIDTH); x++)
            {
                // Get block ID
                uint8_t blockID = chunk.getBlock(x, y, z);
                // If block is air, skip
                if (blockID == 0)
                    continue;
                // If block is not air, iterate through faces
                for (const Face &face : cubeFaces)
                {
                    // Indices first to not affect the base index
                    unsigned int baseIndex = static_cast<unsigned int>(dumbMesh.vertices.size());
                    // Triangle 1
                    dumbMesh.indices.push_back(baseIndex + 0);
                    dumbMesh.indices.push_back(baseIndex + 1);
                    dumbMesh.indices.push_back(baseIndex + 2);
                    // Triangle 2
                    dumbMesh.indices.push_back(baseIndex + 2);
                    dumbMesh.indices.push_back(baseIndex + 3);
                    dumbMesh.indices.push_back(baseIndex + 0);

                    // Vertices second, now base index can be modified
                    std::array<glm::vec3, 4> faceVertices;
                    // Calculate face vertices (offset + real chunk position)
                    faceVertices[0] = face.offsets[0] + glm::vec3(x, y, z);
                    faceVertices[1] = face.offsets[1] + glm::vec3(x, y, z);
                    faceVertices[2] = face.offsets[2] + glm::vec3(x, y, z);
                    faceVertices[3] = face.offsets[3] + glm::vec3(x, y, z);
                    // Push vertices to the end of the vector
                    // texCoords default to (0, 0) until texturing is implemented
                    dumbMesh.vertices.push_back(Vertex{faceVertices[0], face.normal, glm::vec2(0.0f)});
                    dumbMesh.vertices.push_back(Vertex{faceVertices[1], face.normal, glm::vec2(0.0f)});
                    dumbMesh.vertices.push_back(Vertex{faceVertices[2], face.normal, glm::vec2(0.0f)});
                    dumbMesh.vertices.push_back(Vertex{faceVertices[3], face.normal, glm::vec2(0.0f)});
                }
            }

    return dumbMesh;
}

// Face culling (draws)
ChunkMesh ChunkMesher::generateCulledMesh(const Chunk &chunk)
{
    ChunkMesh culledMesh;
    // Same philosophy
    for (int y = 0; y < static_cast<int>(Config::World::CHUNK_HEIGHT); y++)
        for (int z = 0; z < static_cast<int>(Config::World::CHUNK_DEPTH); z++)
            for (int x = 0; x < static_cast<int>(Config::World::CHUNK_WIDTH); x++)
            {
                uint8_t blockID = chunk.getBlock(x, y, z);
                if (blockID == 0)
                    continue;
                for (const Face &face : cubeFaces)
                {
                    // CHANGE: check neighbouring face
                    uint8_t neighbourID = chunk.getBlock(x + face.neighbourOffset.x,
                                                         y + face.neighbourOffset.y,
                                                         z + face.neighbourOffset.z);
                    // If the neighbouring face is solid:
                    // - This face cannot be seen
                    // - There is no point in drawing it
                    // - So skip it!
                    if (BlockDatabase::get(neighbourID).isSolid)
                        continue;

                    // If the neighbouring face is transparent;
                    // - This face can be seen
                    // - It needs to be drawn
                    // - So send data!
                    // Same as before:
                    unsigned int baseIndex = static_cast<unsigned int>(culledMesh.vertices.size());
                    culledMesh.indices.push_back(baseIndex + 0);
                    culledMesh.indices.push_back(baseIndex + 1);
                    culledMesh.indices.push_back(baseIndex + 2);
                    culledMesh.indices.push_back(baseIndex + 2);
                    culledMesh.indices.push_back(baseIndex + 3);
                    culledMesh.indices.push_back(baseIndex + 0);

                    std::array<glm::vec3, 4> faceVertices;
                    faceVertices[0] = face.offsets[0] + glm::vec3(x, y, z);
                    faceVertices[1] = face.offsets[1] + glm::vec3(x, y, z);
                    faceVertices[2] = face.offsets[2] + glm::vec3(x, y, z);
                    faceVertices[3] = face.offsets[3] + glm::vec3(x, y, z);
                    culledMesh.vertices.push_back(Vertex{faceVertices[0], face.normal, glm::vec2(0.0f)});
                    culledMesh.vertices.push_back(Vertex{faceVertices[1], face.normal, glm::vec2(0.0f)});
                    culledMesh.vertices.push_back(Vertex{faceVertices[2], face.normal, glm::vec2(0.0f)});
                    culledMesh.vertices.push_back(Vertex{faceVertices[3], face.normal, glm::vec2(0.0f)});
                }
            }

    return culledMesh;
}