#include "core/Camera.hpp"
#include "core/Constants.hpp"
#include "core/Input.hpp"
#include "core/Timer.hpp"
#include "core/Window.hpp"
#include "renderer/ChunkMesher.hpp"
#include "renderer/ChunkRenderer.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Texture.hpp"
#include "world/Block.hpp"
#include "world/Chunk.hpp"
#include "world/ChunkManager.hpp"
#include "world/WorldGen.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Maybe temporary?
// Must match the actual startup state: the window is created windowed
bool fullscreen = false;

void processInput(Input &input, Window &window)
{
    // Exit program
    if (input.isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window.getNativeWindow(), true);

    // Toggle fullscreen on the press edge, not every frame the key is held
    if (input.isKeyJustPressed(GLFW_KEY_F11))
    {
        if (fullscreen)
            window.fullscreenOff(Config::Window::WIDTH, Config::Window::HEIGHT);
        else
            window.fullscreenOn();

        fullscreen = !fullscreen;
    }
}

void showFPS(Window &window, float fps, bool option)
{
    if (option)
    {
        // Set title: convert float FPS to string
        std::string title = "VoxelCPP - FPS: " + std::to_string(fps);
        glfwSetWindowTitle(window.getNativeWindow(), title.c_str());
    }
}

int main()
{
    // Safe stack initialization
    Window window(Config::Window::WIDTH, Config::Window::HEIGHT, "VoxelCPP");
    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Timer timer;
    Input input(window);
    Camera camera;

    // IMPORTANT: must initialize the block database
    BlockDatabase::init();

    // Initialize textures
    Texture texture("assets/textures/atlas.png");

    // Trying chunk rendering!
    // EDIT 1: Now with noise!
    // EDIT 2: Multiple chunks!
    // EDIT 3: Chunk manager!
    ChunkManager chunkManager;
    chunkManager.addChunk(WorldGen::generateChunk(glm::ivec3(0, 0, 0), WorldGen::GenerationType::Simplex2D));
    chunkManager.addChunk(WorldGen::generateChunk(glm::ivec3(1, 0, 0), WorldGen::GenerationType::Simplex2D));
    chunkManager.addChunk(WorldGen::generateChunk(glm::ivec3(0, 0, 1), WorldGen::GenerationType::Simplex2D));
    chunkManager.addChunk(WorldGen::generateChunk(glm::ivec3(1, 0, 1), WorldGen::GenerationType::Simplex2D));

    // Renderer HashMap
    std::unordered_map<glm::ivec3, std::unique_ptr<ChunkRenderer>, IVec3Hash> chunkRenderers;

    // Range based for to iterate through every loaded chunk
    for (const auto &[position, chunk] : chunkManager)
    {
        // Mesh chunk and update mesh
        ChunkMesh chunkMesh = ChunkMesher::generateCulledMesh(chunk, chunkManager);
        chunkRenderers[position] = std::make_unique<ChunkRenderer>();
        chunkRenderers[position]->updateMesh(chunkMesh);
    }

    // Set VSync on
    window.setVSync(true);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Send static matrices and uniforms
    // Chunks only translate, so the normal matrix stays identity for all of them
    glm::mat3 normalMatrix = glm::mat3(1.0f);

    // Cached chunk dimensions, used to translate each chunk to world space
    glm::ivec3 chunkSize(static_cast<int>(Config::World::CHUNK_WIDTH),
                         static_cast<int>(Config::World::CHUNK_HEIGHT),
                         static_cast<int>(Config::World::CHUNK_DEPTH));

    while (!window.shouldClose())
    {
        // Process all pending events
        window.pollEvents();

        // Update current, delta, and last time
        float fps = timer.update();
        showFPS(window, fps, true);

        // Update vectors
        camera.update(input, timer.getDeltaTime());

        // Clear Buffer Bit and Depth Bit
        window.clear(Config::Renderer::SKY_R,
                     Config::Renderer::SKY_G,
                     Config::Renderer::SKY_B,
                     Config::Renderer::SKY_A);
        // Exit program and fullscreen (temporary solution)
        processInput(input, window);

        // Use shader program to draw cube
        shader.bind();

        // Send non-static matrices and uniforms
        glm::mat4 view = camera.getViewMatrix();
        float aspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
        glm::mat4 projection = glm::perspective(glm::radians(Config::Camera::FOV),
                                                aspectRatio,
                                                Config::Camera::NEAR_PLANE,
                                                Config::Camera::FAR_PLANE);
        shader.setMat3("normalMatrix", normalMatrix);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Draw each chunk at its world position
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        texture.bind(0);
        shader.setInt("texture1", 0);
        for (const auto &[position, renderer] : chunkRenderers)
        {
            // Mesh vertices are local (0..16), so translate the chunk into world space
            glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                             glm::vec3(position * chunkSize));
            shader.setMat4("model", model);
            renderer->draw();
        }

        // Prevent flickering
        window.swapBuffers();

        // Convert current keys to last keys
        // Reset mouse deltas
        input.update();
    }

    return EXIT_SUCCESS;
}