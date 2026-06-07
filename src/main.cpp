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

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Maybe temporary?
// Must match the actual startup state: the window is created windowed
bool fullscreen = false;

// Debugging
unsigned int iterationCount = 0;

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
    Chunk chunk(glm::ivec3(0, 0, 0));
    for (int x = 0; x < static_cast<int>(Config::World::CHUNK_WIDTH); x++)
        for (int z = 0; z < static_cast<int>(Config::World::CHUNK_DEPTH); z++)
            for (int y = 0; y < 7; y++)
            {
                if (y == 6)
                    chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Grass));
                else if (y == 5)
                    chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Dirt));
                else
                    chunk.setBlock(x, y, z, static_cast<uint8_t>(BlockType::Stone));
            }

    // Generate a dumb mesh (everything draws)
    // ChunkMesh dumbMesh = ChunkMesher::generateDumbMesh(chunk);
    // Generate a culled (smarter) mesh (only visible faces draw)
    ChunkMesh culledMesh = ChunkMesher::generateCulledMesh(chunk);

    // Update mesh
    ChunkRenderer chunkRenderer;
    // chunkRenderer.updateMesh(dumbMesh);
    chunkRenderer.updateMesh(culledMesh);

    // Set VSync on
    window.setVSync(true);

    glEnable(GL_DEPTH_TEST);

    // Debug
    std::cout << "DEBUG: Window width is " << window.getWidth() << ".\n";
    std::cout << "DEBUG: Window height is " << window.getHeight() << ".\n";

    // Send static matrices and uniforms
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

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
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Draw chunk
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        texture.bind(0);
        shader.setInt("texture1", 0);
        chunkRenderer.draw();

        // Prevent flickering
        window.swapBuffers();

        // Convert current keys to last keys
        // Reset mouse deltas
        input.update();
    }

    return EXIT_SUCCESS;
}