#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr int SRC_WIDTH = 800;
constexpr int SRC_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

int main()
{
    // =========================
    // 1. Initialize GLFW
    // =========================
    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW.\n";
        return EXIT_FAILURE;
    }

    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // =========================
    // 2. Create window
    // =========================
    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH,
                                          SRC_HEIGHT,
                                          "VoxelCPP",
                                          NULL,
                                          NULL);

    if (window == nullptr)
    {
        std::cerr << "Could not create GLFW window.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Set current context
    glfwMakeContextCurrent(window);

    // =========================
    // 3. Initialize GLAD
    // =========================
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Could not initialize GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // =========================
    // 4. Viewport (resizing)
    // =========================
    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    framebuffer_size_callback(window, fbW, fbH);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // =========================
    // 5. Main loop
    // =========================
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}