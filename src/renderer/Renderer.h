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

    DELETE_MOVE_COPY(Renderer);

    void RecordVkCommand(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags,
                         std::function<void()>&& lambda);

private:
    void CreateInstance();
    void CreateDebugCallback();
    void CreateSurface(GLFWwindow* window);
    void CreateDevice();
    void CreateSwapchain();
    void CreateCommandBufferPool();
    void CreateCommandBuffers(U32 count);
    void RecordCommandBuffers();

    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    U32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities);
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                VkImageViewType viewType, U32 layerCount, U32 mipLevels);

private:
    SharedPtr<Window> m_Window;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkCommandPool m_CommandBufferPool = VK_NULL_HANDLE;

    PhysicalDevices m_PhysicalDevices;

    U32 m_QueueFamily = 0;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;
    std::vector<VkCommandBuffer> m_CommandBuffers;
};
} // namespace FFV
