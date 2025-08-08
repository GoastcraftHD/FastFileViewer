#pragma once

#include "Window.h"
#include "renderer/PhysicalDevice.h"
#include "util/Types.h"
#include "vulkan/vulkan.h"

#include <vector>
namespace FFV
{
class Swapchain
{
public:
    Swapchain(VkDevice device, SharedPtr<PhysicalDevices> physicalDevices, SharedPtr<Window> window, VkSurfaceKHR surface,
              U32 queueFamily);
    ~Swapchain();

    FFV_DELETE_MOVE_COPY(Swapchain);

    void Recreate();

    const VkExtent2D& GetExtent() const { return m_Extent; }
    const std::vector<VkImage>& GetImages() const { return m_Images; };
    const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }
    const VkSwapchainKHR& GetSwapchain() const { return m_Swapchain; }
    U32 GetNumImagesInFlight() const { return m_ImagesInFlight; }

private:
    void CreateSwapchain();
    void CreateImageViews();
    void CleanSwapchain();

    VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const;
    U32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities) const;
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) const;
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                VkImageViewType viewType, U32 layerCount, U32 mipLevels) const;

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    SharedPtr<PhysicalDevices> m_PhysicalDevices;
    SharedPtr<Window> m_Window;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    U32 m_QueueFamily = 0;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkExtent2D m_Extent = { 0, 0 };
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;

    VkSurfaceFormatKHR m_SurfaceFormat;
    U32 m_ImagesInFlight = 0;
};
} // namespace FFV
