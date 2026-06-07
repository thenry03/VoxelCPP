#include "../world/Block.hpp"
#include "BlockTextureMap.hpp"
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
    std::array<glm::vec2, 4> uvs;     // UV base coords [0, 1]
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
        // UVs
        {
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f)},
        // Normal and neighbour direction
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::ivec3(0, 0, -1)},
    // South (+Z)
    {
        {glm::vec3(0.0f, 0.0f, 1.0f),
         glm::vec3(1.0f, 0.0f, 1.0f),
         glm::vec3(1.0f, 1.0f, 1.0f),
         glm::vec3(0.0f, 1.0f, 1.0f)},
        {glm::vec2(0.0f, 0.0f),
         glm::vec2(1.0f, 0.0f),
         glm::vec2(1.0f, 1.0f),
         glm::vec2(0.0f, 1.0f)},
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::ivec3(0, 0, 1)},
    // East (+X)
    {
        {glm::vec3(1.0f, 0.0f, 0.0f),
         glm::vec3(1.0f, 1.0f, 0.0f),
         glm::vec3(1.0f, 1.0f, 1.0f),
         glm::vec3(1.0f, 0.0f, 1.0f)},
        {glm::vec2(1.0f, 0.0f),
         glm::vec2(1.0f, 1.0f),
         glm::vec2(0.0f, 1.0f),
         glm::vec2(0.0f, 0.0f)},
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::ivec3(1, 0, 0)},
    // West (-X)
    {
        {glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(0.0f, 0.0f, 1.0f),
         glm::vec3(0.0f, 1.0f, 1.0f),
         glm::vec3(0.0f, 1.0f, 0.0f)},
        {glm::vec2(0.0f, 0.0f),
         glm::vec2(1.0f, 0.0f),
         glm::vec2(1.0f, 1.0f),
         glm::vec2(0.0f, 1.0f)},
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::ivec3(-1, 0, 0)},
    // Top (+Y)
    {
        {glm::vec3(0.0f, 1.0f, 1.0f),
         glm::vec3(1.0f, 1.0f, 1.0f),
         glm::vec3(1.0f, 1.0f, 0.0f),
         glm::vec3(0.0f, 1.0f, 0.0f)},
        {glm::vec2(0.0f, 0.0f),
         glm::vec2(1.0f, 0.0f),
         glm::vec2(1.0f, 1.0f),
         glm::vec2(0.0f, 1.0f)},
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::ivec3(0, 1, 0)},
    // Bottom (-Y)
    {
        {glm::vec3(0.0f, 0.0f, 1.0f),
         glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(1.0f, 0.0f, 0.0f),
         glm::vec3(1.0f, 0.0f, 1.0f)},
        {glm::vec2(0.0f, 0.0f),
         glm::vec2(0.0f, 1.0f),
         glm::vec2(1.0f, 1.0f),
         glm::vec2(1.0f, 0.0f)},
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

                    // This part is different: textures!
                    // Get the texture layout
                    BlockTextureLayout textureLayout = blockTextureMap[blockID];
                    uint8_t textureID;

                    // If normal is +Y, it is the top face
                    // If normal is -Y, it is the bottom face
                    // Anything else is a side face trated equally
                    if (face.normal.y > 0.0f)
                        textureID = textureLayout.top;
                    else if (face.normal.y < 0.0f)
                        textureID = textureLayout.bottom;
                    else
                        textureID = textureLayout.side;

                    // Convert textureID to UV coords using 8x8 atlas math
                    // Each tile occupies (size x size) in UV space, where size = 1/8 = 0.125
                    // UV origin (0,0) is bottom-left; atlas origin is top-left, so V is flipped
                    //
                    // Nice example: Grass block top (textureID = 0)
                    // column = 0 % 8 = 0
                    // row    = 0 / 8 = 0
                    // uMin   = 0 * 0.125f = 0.0f
                    // vMin   = 1.0f - (0 * 0.125f) - 0.125f = 0.875f
                    //
                    // Face UVs are then scaled from [0,1] into [uMin, uMin+size] x [vMin, vMin+size]:
                    // uv = (uMin + face.uvs[i].x * size, vMin + face.uvs[i].y * size)
                    float size = Config::Renderer::TEXTURE_UV_SIZE;
                    int column = textureID % 8;
                    int row = textureID / 8;
                    float uMin = column * size;
                    float vMin = 1.0f - (row * size) - size;

                    // Map the corners in CCW order
                    glm::vec2 uv0 = glm::vec2(uMin + face.uvs[0].x * size, vMin + face.uvs[0].y * size);
                    glm::vec2 uv1 = glm::vec2(uMin + face.uvs[1].x * size, vMin + face.uvs[1].y * size);
                    glm::vec2 uv2 = glm::vec2(uMin + face.uvs[2].x * size, vMin + face.uvs[2].y * size);
                    glm::vec2 uv3 = glm::vec2(uMin + face.uvs[3].x * size, vMin + face.uvs[3].y * size);

                    // Push vertices with their newly calculated UVs
                    culledMesh.vertices.push_back(Vertex{faceVertices[0], face.normal, uv0});
                    culledMesh.vertices.push_back(Vertex{faceVertices[1], face.normal, uv1});
                    culledMesh.vertices.push_back(Vertex{faceVertices[2], face.normal, uv2});
                    culledMesh.vertices.push_back(Vertex{faceVertices[3], face.normal, uv3});
                }
            }

    return culledMesh;
}