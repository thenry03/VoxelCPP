#include "core/Camera.hpp"
#include "core/Constants.hpp"
#include "core/Input.hpp"
#include "core/Timer.hpp"
#include "core/Window.hpp"
#include "renderer/ChunkMesher.hpp"
#include "renderer/ChunkRenderer.hpp"
#include "renderer/Shader.hpp"
#include "renderer/SunRenderer.hpp"
#include "renderer/Texture.hpp"
#include "scene/actors/Sun.hpp"
#include "scene/systems/ChunkMeshingSystem.hpp"
#include "world/blocks/Block.hpp"
#include "world/chunks/Chunk.hpp"
#include "world/chunks/ChunkManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <memory>
#include <string>
#include <unordered_map>

// Temporary
constexpr int NUMBER_OF_ITERATIONS = 8;

// Tracks fullscreen state; must match the initial window creation mode
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

    // ==========================================
    // COMPLETE CHUNK SYSTEM
    // ==========================================
    // Chunk manager: changes states, notifies meshing
    ChunkManager chunkManager;
    // Create the chunk meshing system and assign its manager
    ChunkMeshingSystem chunkMeshingSystem(chunkManager);

    // The manager must notify the meshing system when a chunk is ready for meshing
    chunkManager.setOnChunkGenerated(
        [&chunkMeshingSystem](const glm::ivec3 &position)
        { chunkMeshingSystem.enqueue(position); });

    // The manager must ask the meshing system to re-mesh neighbours when a chunk turns Ready
    chunkManager.setOnChunkReady(
        [&chunkMeshingSystem](const glm::ivec3 &position)
        { chunkMeshingSystem.enqueue(position); });

    // One GPU renderer per chunk, keyed by chunk position
    std::unordered_map<glm::ivec3, std::unique_ptr<ChunkRenderer>, IVec3Hash> chunkRenderers;

    // The manager must ask the render system to unload unseen chunks
    chunkManager.setOnChunkUnloaded(
        [&chunkRenderers](const glm::ivec3 &position)
        { chunkRenderers.erase(position); });

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

    // Create Sun and its renderer
    Sun sun;
    SunRenderer sunRenderer;

    // GAME LOOP
    while (!window.shouldClose())
    {
        // Process all pending events
        window.pollEvents();

        // Update current, delta, and last time
        float fps = timer.update();
        showFPS(window, fps, true);

        // Update camera from input
        camera.update(input, timer.getDeltaTime());

        // Clear Buffer Bit and Depth Bit
        window.clear(Config::Renderer::SKY_R,
                     Config::Renderer::SKY_G,
                     Config::Renderer::SKY_B,
                     Config::Renderer::SKY_A);
        // Exit program and fullscreen (temporary solution)
        processInput(input, window);

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

        // Before consuming meshes, update list of observable chunks
        chunkManager.update(camera.getPlayerPosition(),
                            static_cast<int>(Config::World::RENDER_DISTANCE));

        // Upload ready meshes to GPU
        for (auto &[position, mesh] : chunkMeshingSystem.consumeReadyMeshes())
        {
            // Skip meshes for chunks unloaded while queued, else we resurrect an
            // orphan renderer the unload pass can no longer reach
            if (!chunkManager.hasChunk(position))
                continue;
            if (chunkRenderers.find(position) == chunkRenderers.end())
                // If ChunkRenderer doesn't exist for given position, create it
                chunkRenderers[position] = std::make_unique<ChunkRenderer>();
            chunkRenderers[position]->updateMesh(mesh);
        }

        // Draw each chunk at its world position
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

        // Draw Sun
        // Treat its position as a sky direction anchored to the camera, so it
        // keeps its apparent place and always sits inside the far plane
        glm::vec3 sunDirection = glm::normalize(sun.getSunPosition());
        glm::vec3 sunWorldPosition = camera.getPlayerPosition() + sunDirection * 1500.0f;
        sunRenderer.draw(sunWorldPosition,
                         sun.getSunColor(),
                         sun.getSunSize(),
                         view,
                         projection);

        // Prevent flickering
        window.swapBuffers();

        // Convert current keys to last keys
        // Reset mouse deltas
        input.update();
    }

    // Stop the generation worker first so it can no longer fire the callback
    // into the meshing system, then stop the meshing worker
    chunkManager.stop();
    chunkMeshingSystem.stop();

    return EXIT_SUCCESS;
}