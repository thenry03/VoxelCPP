#include "Window.hpp"

#include <glad/glad.h>

#include <stdexcept>

// ==========================================
// 1. LIFECYCLE
// ==========================================
Window::Window(int width, int height, const std::string &title)
    : m_width(width), m_height(height)
{
    // =========================
    // 1. Initialize GLFW
    // =========================
    if (!glfwInit())
    {
        throw std::runtime_error("Could not initialize GLFW.");
    }

    // Configure OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // =========================
    // 2. Create window
    // =========================
    m_window = glfwCreateWindow(m_width,
                                m_height,
                                title.c_str(),
                                nullptr,
                                nullptr);
    if (m_window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Could not create GLFW window.");
    }

    // =========================
    // 3. Create monitor
    // =========================
    m_monitor = glfwGetPrimaryMonitor();
    m_mode = glfwGetVideoMode(m_monitor);

    if (m_monitor == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Could not create GLFW monitor.");
    }

    // Make current context
    glfwMakeContextCurrent(m_window);

    // =========================
    // 4. Initialize GLAD
    // =========================
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Could not initialize GLAD.");
    }

    // =========================
    // 5. Register callbacks
    // =========================
    // Save this object in GLFW's window
    glfwSetWindowUserPointer(m_window, this);

    // Register key callback
    glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       {
        (void)scancode;
        (void) mods;
        // Bridge the gap between GLFW C-style event system and object oriented Window class
        // This line retrieves the C++ Window instance from the GLFW window handle
        // This way the static callback can execute methods on the actual object
        Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (instance)
            instance->m_eventDispatcher.dispatchKey(key, action); });

    // Register cursor position callback
    glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double xPos, double yPos)
                             {
        Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (instance)
            instance->m_eventDispatcher.dispatchMouse((float)xPos, (float)yPos); });

    // Configure initial viewport
    int fbW, fbH;
    glfwGetFramebufferSize(m_window, &fbW, &fbH);
    // Makes getWidth() and getHeight() always return framebuffer size
    m_width = fbW;
    m_height = fbH;
    glViewport(0, 0, m_width, m_height);
    // Register resize callback
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    // Subscribe onResize to dispatcher
    m_eventDispatcher.onResize([this](int width, int height)
                               { onResize(width, height); });
}

Window::~Window()
{
    if (m_window)
        glfwDestroyWindow(m_window);
    glfwTerminate();
}

// ==========================================
// 2. PUBLIC METHODS
// ==========================================
bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

int Window::getWidth() const
{
    return m_width;
}

int Window::getHeight() const
{
    return m_height;
}

GLFWwindow *Window::getNativeWindow() const
{
    return m_window;
}

EventDispatcher &Window::getEventDispatcher()
{
    return m_eventDispatcher;
}

void Window::fullscreenOn()
{
    glfwSetWindowMonitor(m_window,
                         m_monitor,
                         0,
                         0,
                         m_mode->width,
                         m_mode->height,
                         m_mode->refreshRate);
}

void Window::fullscreenOff(int width, int height)
{
    glfwSetWindowMonitor(m_window,
                         NULL,
                         100,
                         100,
                         width,
                         height,
                         GLFW_DONT_CARE);
}

void Window::setVSync(bool enabled) const
{
    glfwSwapInterval(static_cast<int>(enabled));
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::clear(float r,
                   float g,
                   float b,
                   float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// ==========================================
// 3. PRIVATE METHODS
// ==========================================
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));

    if (instance)
        instance->m_eventDispatcher.dispatchResize(width, height);
}

void Window::onResize(int width, int height)
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);
}