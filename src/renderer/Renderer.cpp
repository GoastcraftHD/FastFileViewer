#include "FastFileViewerPCH.h"

#include "Renderer.h"

#include "GLFW/glfw3.h"
#include "util/Log.h"
#include "util/Util.h"
#include "vulkan/vulkan_core.h"

namespace FFV
{
Renderer::Renderer(SharedPtr<Window> window) : m_Window(window)
{
    CreateInstance();
#if defined(FFV_DEBUG)
    CreateDebugCallback();
#endif
    CreateSurface(m_Window->GetNativeWindow());
    m_PhysicalDevices = PhysicalDevices(m_Instance, m_Surface);
    m_QueueFamily = m_PhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
    CreateDevice();
    CreateSwapchain();
    CreateCommandBufferPool();
    CreateCommandBuffers(static_cast<U32>(m_ImageViews.size()));
    RecordCommandBuffers();

    m_Queue = Queue(m_Device, m_Swapchain, m_QueueFamily, 0);
}

Renderer::~Renderer()
{
    vkFreeCommandBuffers(m_Device, m_CommandBufferPool, m_CommandBuffers.size(), m_CommandBuffers.data());
    vkDestroyCommandPool(m_Device, m_CommandBufferPool, VK_NULL_HANDLE);

    for (U32 i = 0; i < m_ImageViews.size(); i++)
    {
        vkDestroyImageView(m_Device, m_ImageViews[i], VK_NULL_HANDLE);
    }

    vkDestroySwapchainKHR(m_Device, m_Swapchain, VK_NULL_HANDLE);

    vkDestroyDevice(m_Device, VK_NULL_HANDLE);

    const PFN_vkDestroySurfaceKHR vkDestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(m_Instance, "vkDestroySurfaceKHR"));
    FFV_ASSERT(vkDestroySurface, "Cannot find address of vkDestroySurface", ;);
    vkDestroySurface(m_Instance, m_Surface, VK_NULL_HANDLE);

#if defined(FFV_DEBUG)
    const PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
    FFV_ASSERT(vkDestroyDebugUtilsMessenger, "Cannot find address of vkDestroyDebugUtilsMessenger", ;);
    vkDestroyDebugUtilsMessenger(m_Instance, m_DebugMessenger, VK_NULL_HANDLE);
#endif

    vkDestroyInstance(m_Instance, VK_NULL_HANDLE);
}

void Renderer::RecordVkCommand(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags,
                               std::function<void()>&& lambda)
{
    VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = usageFlags };

    FFV_CHECK_VK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    lambda();

    FFV_CHECK_VK_RESULT(vkEndCommandBuffer(commandBuffer));
}

void Renderer::Update()
{
    U32 imageIndex = m_Queue.AquireNextImage();
    m_Queue.SubmitAsync(m_CommandBuffers[imageIndex]);
    m_Queue.Present(imageIndex);
}

void Renderer::CreateInstance()
{
    const std::vector<const char*> layers = {
#if defined(FFV_DEBUG)
        "VK_LAYER_KHRONOS_validation"
#endif
    };

    const std::vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(FFV_LINUX)
                                                  "VK_KHR_xcb_surface",
#elif defined(FFV_WINDOWS)
                                            "VK_KHR_win32_surface",
#endif
#if defined(FFV_DEBUG)
                                                  VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
    };

    const VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                        .pApplicationName = "Fast File Viewer",
                                        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                                        .pEngineName = "Fast",
                                        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                                        .apiVersion = VK_API_VERSION_1_3 };

    const VkInstanceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                              .pApplicationInfo = &appInfo,
                                              .enabledLayerCount = static_cast<U32>(layers.size()),
                                              .ppEnabledLayerNames = layers.data(),
                                              .enabledExtensionCount = static_cast<U32>(extensions.size()),
                                              .ppEnabledExtensionNames = extensions.data() };

    FFV_CHECK_VK_RESULT(vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_Instance));
    FFV_TRACE("Created vulkan instance!");
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*)
{
    std::string typeStr = "";
    switch (type)
    {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            typeStr = "General";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            typeStr = "Validation";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            typeStr = "Performance";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
            typeStr = "Device address binding";
            break;
        default:
            std::unreachable();
    }

#define VULKAN_DEBUG_MESSAGE         \
    "Vulkan debug callback:\n"       \
    "                  Type: {0}\n"  \
    "                  Objects: {1}" \
    "                  Message: {2}"

    std::stringstream ss;
    for (U32 i = 0; i < callbackData->objectCount; i++)
    {
        ss << callbackData->pObjects[i].objectHandle << "\n";
    }

    switch (severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            FFV_TRACE(VULKAN_DEBUG_MESSAGE, typeStr, ss.str(), callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            FFV_LOG(VULKAN_DEBUG_MESSAGE, typeStr, ss.str(), callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            FFV_WARN(VULKAN_DEBUG_MESSAGE, typeStr, ss.str(), callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            FFV_ERROR(VULKAN_DEBUG_MESSAGE, typeStr, ss.str(), callbackData->pMessage);
            break;
        default:
            std::unreachable();
    }

#undef VULKAN_DEBUG_MESSAGE

    return VK_FALSE;
}

void Renderer::CreateDebugCallback()
{
    const VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &DebugCallback
    };

    const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
    FFV_ASSERT(vkCreateDebugUtilsMessenger, "Cannot find address of vkCreateDebugUtilsMessenger", return);
    FFV_CHECK_VK_RESULT(vkCreateDebugUtilsMessenger(m_Instance, &messengerCreateInfo, VK_NULL_HANDLE, &m_DebugMessenger));
    FFV_TRACE("Created vulkan debug callback!");
}

void Renderer::CreateSurface(GLFWwindow* window)
{
    FFV_CHECK_VK_RESULT(glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface));
    FFV_TRACE("Created GLFW window surface!");
}

void Renderer::CreateDevice()
{
    float queuePriorities[] = { 1.0f };

    const VkDeviceQueueCreateInfo queueCreateInfo = { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                      .queueFamilyIndex = m_QueueFamily,
                                                      .queueCount = 1,
                                                      .pQueuePriorities = &queuePriorities[0] };

    const std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                  VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME };

    FFV_ASSERT(m_PhysicalDevices.GetSelectedPhysicalDevice().Features.geometryShader != VK_FALSE,
               "Device does not support Geometry Shaders!", exit(1));
    FFV_ASSERT(m_PhysicalDevices.GetSelectedPhysicalDevice().Features.tessellationShader != VK_FALSE,
               "Device does not support Tessellation Shaders!", exit(1));

    VkPhysicalDeviceFeatures deviceFeatures = { 0 };
    deviceFeatures.geometryShader = VK_TRUE;
    deviceFeatures.tessellationShader = VK_TRUE;

    const VkDeviceCreateInfo deviceCreateInfo = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                                  .queueCreateInfoCount = 1,
                                                  .pQueueCreateInfos = &queueCreateInfo,
                                                  .enabledExtensionCount = static_cast<U32>(extensions.size()),
                                                  .ppEnabledExtensionNames = extensions.data(),
                                                  .pEnabledFeatures = &deviceFeatures };

    FFV_CHECK_VK_RESULT(vkCreateDevice(m_PhysicalDevices.GetSelectedPhysicalDevice().PhysicalDevice, &deviceCreateInfo,
                                       VK_NULL_HANDLE, &m_Device));

    FFV_TRACE("Created vulkan device!");
}

void Renderer::CreateSwapchain()
{
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = m_PhysicalDevices.GetSelectedPhysicalDevice().SurfaceCapabilities;

    const U32 numImages = ChooseNumImages(surfaceCapabilities);

    const std::vector<VkPresentModeKHR>& presentModes = m_PhysicalDevices.GetSelectedPhysicalDevice().PresentModes;
    const VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);

    const VkSurfaceFormatKHR surfaceFormat =
        ChooseSurfaceFormatAndColorSpace(m_PhysicalDevices.GetSelectedPhysicalDevice().SurfaceFormats);

    const VkSwapchainCreateInfoKHR swapchainCreateInfo = { .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                           .surface = m_Surface,
                                                           .minImageCount = numImages,
                                                           .imageFormat = surfaceFormat.format,
                                                           .imageColorSpace = surfaceFormat.colorSpace,
                                                           .imageExtent = surfaceCapabilities.currentExtent,
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

    U32 numSwapchainImages = 0;
    FFV_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &numSwapchainImages, VK_NULL_HANDLE));
    FFV_ASSERT(numImages == numSwapchainImages, "Swapchain image number doesn't match requested image number!", ;);

    m_Images.resize(numSwapchainImages);
    m_ImageViews.resize(numSwapchainImages);

    FFV_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &numSwapchainImages, m_Images.data()));

    const U32 layerCount = 1;
    const U32 mipLevels = 1;

    for (U32 i = 0; i < numSwapchainImages; i++)
    {
        m_ImageViews[i] = CreateImageView(m_Device, m_Images[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT,
                                          VK_IMAGE_VIEW_TYPE_2D, layerCount, mipLevels);
    }
}

void Renderer::CreateCommandBufferPool()
{
    const VkCommandPoolCreateInfo commandBufferCreateInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                              .queueFamilyIndex = m_QueueFamily };

    FFV_CHECK_VK_RESULT(vkCreateCommandPool(m_Device, &commandBufferCreateInfo, VK_NULL_HANDLE, &m_CommandBufferPool));
    FFV_TRACE("Created command pool!");
}

void Renderer::CreateCommandBuffers(U32 count)
{
    const VkCommandBufferAllocateInfo commandBufferAllocateInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                    .commandPool = m_CommandBufferPool,
                                                                    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                    .commandBufferCount = count };

    m_CommandBuffers.resize(count);

    FFV_CHECK_VK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, m_CommandBuffers.data()));
    FFV_TRACE("Created {0} command buffers!", count);
}

void Renderer::RecordCommandBuffers()
{
    const VkClearColorValue clearColor = {
        .float32 = { 1.0f, 0.0f, 0.0f, 0.0f }
    };

    const VkImageSubresourceRange imageSubResourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1
    };

    for (U32 i = 0; i < m_CommandBuffers.size(); i++)
    {
        VkImageMemoryBarrier presentToClearBarrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                       .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                                                       .dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
                                                       .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                       .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .image = m_Images[i],
                                                       .subresourceRange = imageSubResourceRange };

        VkImageMemoryBarrier clearToPresentBarrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                       .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                                       .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                                                       .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                       .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .image = m_Images[i],
                                                       .subresourceRange = imageSubResourceRange };

        RecordVkCommand(m_CommandBuffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                        [&]()
                        {
                            vkCmdPipelineBarrier(m_CommandBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1,
                                                 &presentToClearBarrier);

                            vkCmdClearColorImage(m_CommandBuffers[i], m_Images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                 &clearColor, 1, &imageSubResourceRange);

                            vkCmdPipelineBarrier(m_CommandBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, VK_NULL_HANDLE, 0,
                                                 VK_NULL_HANDLE, 1, &clearToPresentBarrier);
                        });
    }

    FFV_TRACE("Command buffers have been recorded!");
}

VkSurfaceFormatKHR Renderer::ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
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
U32 Renderer::ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities)
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
VkPresentModeKHR Renderer::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
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
VkImageView Renderer::CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                      VkImageViewType viewType, U32 layerCount, U32 mipLevels)
{
    VkImageViewCreateInfo imageViewCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = image,
    .viewType = viewType,
    .format = format,
    .components = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,

    },
    .subresourceRange = {
        .aspectMask = aspectFlags,
        .baseMipLevel = 0,
        .levelCount = mipLevels,
        .baseArrayLayer = 0,
        .layerCount = layerCount
    }
    };

    VkImageView imageView = VK_NULL_HANDLE;
    FFV_CHECK_VK_RESULT(vkCreateImageView(device, &imageViewCreateInfo, VK_NULL_HANDLE, &imageView));

    return imageView;
}
} // namespace FFV
