#include "core/Camera.hpp"
#include "core/Constants.hpp"
#include "core/Input.hpp"
#include "core/Timer.hpp"
#include "core/Window.hpp"
#include "renderer/Shader.hpp"

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
            window.fullscreenOff(WINDOW_WIDTH, WINDOW_HEIGHT);
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

// Define
unsigned int VAO, VBO, EBO;

void defineCube()
{
    static const float vertices[] = {
        // Front
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        // Back
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};

    static const unsigned int indexes[] = {
        // Front
        0, 1, 2,
        0, 2, 3,
        // Right
        1, 5, 6,
        6, 2, 1,
        // Back
        7, 6, 5,
        5, 4, 7,
        // Left
        4, 0, 3,
        3, 7, 4,
        // Bottom
        4, 5, 1,
        1, 0, 4,
        // Top
        3, 2, 6,
        6, 7, 3};

    // Generate
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    // Send data
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind VAO
    glBindVertexArray(0);
}

void drawCube(const Shader &shader, const Window &window, const Camera &camera)
{
    // Transformations
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        (float)window.getWidth() / (float)window.getHeight(),
        0.1f,
        100.0f);

    // Send uniforms
    shader.setMat3("normalMatrix", normalMatrix);
    shader.setMat4("model", model);
    shader.setMat4("view", camera.getViewMatrix());
    shader.setMat4("projection", projection);

    // Draw calls
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

int main()
{
    // Safe stack initialization
    Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "VoxelCPP");
    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Timer timer;
    Input input(window);
    Camera camera;

    // Set VSync on
    window.setVSync(true);

    defineCube();
    glEnable(GL_DEPTH_TEST);

    // Debug
    std::cout << "DEBUG: Window width is " << window.getWidth() << ".\n";
    std::cout << "DEBUG: Window height is " << window.getHeight() << ".\n";

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
        window.clear();

        // Exit program and fullscreen (temporary solution)
        processInput(input, window);

        // Use shader program to draw cube
        shader.bind();
        drawCube(shader, window, camera);

        // Prevent flickering
        window.swapBuffers();

        // Convert current keys to last keys
        // Reset mouse deltas
        input.update();
    }

    return EXIT_SUCCESS;
}