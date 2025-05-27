#pragma once

struct GLFWwindow;

#include "Window.h"
#include "renderer/PhysicalDevice.h"
#include "util/Types.h"

namespace FFV
{
class Renderer
{
public:
    Renderer(SharedPtr<Window> window);
    ~Renderer();

private:
    void CreateInstance();
    void CreateDebugCallback();
    void CreateSurface(GLFWwindow* window);
    void CreateDevice();

private:
    SharedPtr<Window> m_Window;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    PhysicalDevices m_PhysicalDevices;
    VkDevice m_Device;
    U32 m_QueueFamily = 0;
};
} // namespace FFV
