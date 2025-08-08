#include "FastFileViewerPCH.h"

#include "Swapchain.h"

namespace FFV
{
Swapchain::Swapchain(VkDevice device, SharedPtr<PhysicalDevices> physicalDevices, SharedPtr<Window> window,
                     VkSurfaceKHR surface, U32 queueFamily)
    : m_Device(device), m_PhysicalDevices(physicalDevices), m_Window(window), m_Surface(surface), m_QueueFamily(queueFamily)
{
    CreateSwapchain();
    CreateImageViews();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Swapchain::~Swapchain() { CleanSwapchain(); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Swapchain::Recreate()
{
    I32 width = 0;
    I32 height = 0;
    glfwGetFramebufferSize(m_Window->GetNativeWindow(), &width, &height);

    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_Window->GetNativeWindow(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_Device);

    CleanSwapchain();

    CreateSwapchain();
    CreateImageViews();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Swapchain::CreateSwapchain()
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevices->GetSelectedPhysicalDevice().PhysicalDevice, m_Surface,
                                              &surfaceCapabilities);

    m_ImagesInFlight = ChooseNumImages(surfaceCapabilities);

    const std::vector<VkPresentModeKHR>& presentModes = m_PhysicalDevices->GetSelectedPhysicalDevice().PresentModes;
    const VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);

    m_SurfaceFormat = ChooseSurfaceFormatAndColorSpace(m_PhysicalDevices->GetSelectedPhysicalDevice().SurfaceFormats);
    m_Extent = ChooseSwapchainExtent(surfaceCapabilities);

    const VkSwapchainCreateInfoKHR swapchainCreateInfo = { .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                           .surface = m_Surface,
                                                           .minImageCount = m_ImagesInFlight,
                                                           .imageFormat = m_SurfaceFormat.format,
                                                           .imageColorSpace = m_SurfaceFormat.colorSpace,
                                                           .imageExtent = m_Extent,
                                                           .imageArrayLayers = 1,
                                                           .imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                          VK_IMAGE_USAGE_TRANSFER_DST_BIT),
                                                           .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                           .queueFamilyIndexCount = 1,
                                                           .pQueueFamilyIndices = &m_QueueFamily,
                                                           .preTransform = surfaceCapabilities.currentTransform,
                                                           .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                                           .presentMode = presentMode,
                                                           .clipped = VK_TRUE };

    FFV_CHECK_VK_RESULT(vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, VK_NULL_HANDLE, &m_Swapchain));

    FFV_TRACE("Created vulkan swapchain!");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Swapchain::CreateImageViews()
{
    U32 numSwapchainImages = 0;
    FFV_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &numSwapchainImages, VK_NULL_HANDLE));
    FFV_ASSERT(m_ImagesInFlight == numSwapchainImages, "Swapchain image number doesn't match requested image number!", ;);

    m_Images.resize(numSwapchainImages);
    m_ImageViews.resize(numSwapchainImages);

    FFV_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &numSwapchainImages, m_Images.data()));

    const U32 layerCount = 1;
    const U32 mipLevels = 1;

    for (U32 i = 0; i < numSwapchainImages; i++)
    {
        m_ImageViews[i] = CreateImageView(m_Device, m_Images[i], m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT,
                                          VK_IMAGE_VIEW_TYPE_2D, layerCount, mipLevels);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Swapchain::CleanSwapchain()
{
    for (U32 i = 0; i < m_ImageViews.size(); i++)
    {
        vkDestroyImageView(m_Device, m_ImageViews[i], VK_NULL_HANDLE);
    }

    vkDestroySwapchainKHR(m_Device, m_Swapchain, VK_NULL_HANDLE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkExtent2D Swapchain::ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
{

    VkExtent2D extent = { .width = m_Window->GetWidth(), .height = m_Window->GetHeight() };
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const
{
    for (const VkSurfaceFormatKHR& format : surfaceFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    FFV_WARN("No suitable surface format and colorspace found!");

    return surfaceFormats[0];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U32 Swapchain::ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities) const
{
    U32 requestedNumImages = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && requestedNumImages > capabilities.maxImageCount)
    {
        return capabilities.maxImageCount;
    }
    else
    {
        return requestedNumImages;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkPresentModeKHR Swapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) const
{
    for (const VkPresentModeKHR& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkImageView Swapchain::CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                       VkImageViewType viewType, U32 layerCount, U32 mipLevels) const
{
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = viewType,
        .format = format,
        .components = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = { .aspectMask = aspectFlags,
                       .baseMipLevel = 0,
                       .levelCount = mipLevels,
                       .baseArrayLayer = 0,
                       .layerCount = layerCount }
    };

    VkImageView imageView = VK_NULL_HANDLE;
    FFV_CHECK_VK_RESULT(vkCreateImageView(device, &imageViewCreateInfo, VK_NULL_HANDLE, &imageView));

    return imageView;
}
} // namespace FFV
