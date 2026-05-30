#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/Window.hpp"

constexpr int width = 800;
constexpr int height = 600;

int main()
{
    // Safe stack initialization
    Window window(width, height, "VoxelCPP");

    // Set VSync off
    window.setVSync(false);

    while(!window.shouldClose())
    {
        window.clear();

        window.swapBuffers();
        window.pollEvents();
    }

    return EXIT_SUCCESS;
}