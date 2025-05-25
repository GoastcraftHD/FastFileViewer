#pragma once

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

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};
} // namespace FFV
