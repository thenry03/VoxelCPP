#pragma once

#include "EventDispatcher.hpp"

#include <GLFW/glfw3.h>

#include <string>

// RAII wrapper over a GLFW window and its OpenGL context
// Owns the sole glfwSetWindowUserPointer slot, which it uses to route
// GLFW C callbacks back into C++ via EventDispatcher
class Window
{
public:
    // --- Lifecycle ---
    Window(int width, int height, const std::string &title);
    ~Window();

    // --- RAII ---
    Window(const Window &)            = delete;
    Window &operator=(const Window &) = delete;

    // --- State queries ---
    bool shouldClose() const;
    int  getWidth()    const;
    int  getHeight()   const;

    // --- Native pointer access ---
    GLFWwindow *getNativeWindow() const;
    EventDispatcher& getEventDispatcher();
    void fullscreenOn();
    void fullscreenOff(int width, int height);

    // --- Options ---
    void setVSync(bool enabled) const;

    // --- Game loop operations ---
    void swapBuffers()         const;
    void pollEvents()          const;
    void clear(float r = 0.0f,
               float g = 0.0f,
               float b = 0.0f,
               float a = 1.0f) const;

private:
    // --- Private methods ---
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void onResize(int width, int height);

    // --- Private attributes ---
    GLFWwindow        *m_window = nullptr;
    GLFWmonitor       *m_monitor = nullptr;
    const GLFWvidmode *m_mode = nullptr;
    int                m_width = 0;
    int                m_height = 0;
    bool               m_fullscreen = false;
    // Window is the only owner of the user pointer
    // It will dispatch different events
    EventDispatcher m_eventDispatcher;
};