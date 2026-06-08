#pragma once

#include <functional>
#include <vector>

class EventDispatcher
{
public:
    // --- Lifecycle ---
    EventDispatcher()  = default;
    ~EventDispatcher() = default;

    // --- onEvent methods ---
    void onKey(std::function<void(int key, int action)> callback);
    void onMouse(std::function<void(float xPos, float yPos)> callback);
    void onResize(std::function<void(int width, int height)> callback);
    void dispatchKey(int key, int action);
    void dispatchMouse(float xPos, float yPos);
    void dispatchResize(int width, int height);

private:
    // --- Private attributes ---
    std::vector<std::function<void(int key, int action)>> m_keyCallbacks;
    std::vector<std::function<void(float xPos, float yPos)>> m_mouseCallbacks;
    std::vector<std::function<void(int width, int height)>> m_resizeCallbacks;
};