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
    U32 GetNumImagesInFlight() const { return m_ImagesInFlight; }

private:
    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const;
    U32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities) const;
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) const;
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                VkImageViewType viewType, U32 layerCount, U32 mipLevels) const;

private:
    VkDevice m_Device = VK_NULL_HANDLE;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;

    VkSurfaceFormatKHR m_SurfaceFormat;
    U32 m_ImagesInFlight = 0;
};
} // namespace FFV
