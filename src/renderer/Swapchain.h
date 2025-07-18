#pragma once

#include "renderer/PhysicalDevice.h"
#include "util/Types.h"
#include "vulkan/vulkan.h"

#include <vector>

namespace FFV
{
class Swapchain
{
public:
    Swapchain(VkDevice device, SharedPtr<PhysicalDevices> physicalDevices, VkSurfaceKHR surface, U32 queueFamily);
    ~Swapchain();

    FFV_DELETE_MOVE_COPY(Swapchain);

    const std::vector<VkImage>& GetImages() const { return m_Images; };
    const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }
    VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }

private:
    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    U32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities);
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                VkImageViewType viewType, U32 layerCount, U32 mipLevels);

private:
    VkDevice m_Device = VK_NULL_HANDLE;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;

    VkSurfaceFormatKHR m_SurfaceFormat;
};
} // namespace FFV
