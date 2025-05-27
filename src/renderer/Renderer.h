#pragma once

struct GLFWwindow;

#include "renderer/PhysicalDevice.h"

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
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    PhysicalDevices m_PhysicalDevices;
    U32 m_QueueFamily = 0;
};
} // namespace FFV
