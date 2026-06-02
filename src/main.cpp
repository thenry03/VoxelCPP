#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/Window.hpp"
#include "renderer/Shader.hpp"
#include "core/Camera.hpp"
#include "core/Input.hpp"
#include "core/Timer.hpp"

constexpr int width = 800;
constexpr int height = 600;

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
    Window window(width, height, "VoxelCPP");
    Shader shader("shaders/shader.vert", "shaders/shader.frag");

    Timer timer;
    Input input(window.getNativeWindow());
    Camera camera;

    // Set VSync off
    window.setVSync(false);

    defineCube();
    glEnable(GL_DEPTH_TEST);

    while (!window.shouldClose())
    {
        window.clear();

        timer.update();
        input.update();

        if (input.isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window.getNativeWindow(), true);

        camera.update(input, timer.getDeltaTime());

        shader.bind();
        drawCube(shader, window, camera);

        window.swapBuffers();
        window.pollEvents();
    }

    return EXIT_SUCCESS;
}