#include "EventDispatcher.hpp"

// ==========================================
// 1. PUBLIC METHODS
// ==========================================
void EventDispatcher::onKey(
    std::function<void(int key, int action)> callback)
{
    m_keyCallbacks.push_back(callback);
}

void EventDispatcher::onMouse(
    std::function<void(float xPos, float yPos)> callback)
{
    m_mouseCallbacks.push_back(callback);
}

void EventDispatcher::onResize(
    std::function<void(int width, int height)> callback)
{
    m_resizeCallbacks.push_back(callback);
}

void EventDispatcher::dispatchKey(int key, int action)
{
    for (auto &callback : m_keyCallbacks)
        callback(key, action);
}

void EventDispatcher::dispatchMouse(float xPos, float yPos)
{
    for (auto &callback : m_mouseCallbacks)
        callback(xPos, yPos);
}

void EventDispatcher::dispatchResize(int width, int height)
{
    for (auto &callback : m_resizeCallbacks)
        callback(width, height);
}