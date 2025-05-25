#pragma once

#include "GLFW/glfw3.h"

#include <vulkan/vulkan.h>

namespace FFV
{
class Renderer
{
public:
    Renderer();
    ~Renderer();

private:
    void CreateInstance();
    void CreateDebugCallback();
    void CreateSurface(GLFWwindow* window);

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface;
};
} // namespace FFV
