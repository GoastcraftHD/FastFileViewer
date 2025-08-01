#include "FastFileViewerPCH.h"

#include "Renderer.h"

#include "GLFW/glfw3.h"
#include "renderer/Shader.h"
#include "util/Log.h"
#include "util/Types.h"
#include "util/Util.h"
#include "vulkan/vulkan_core.h"

#include <vector>

namespace FFV
{
Renderer::Renderer(SharedPtr<Window> window) : m_Window(window)
{
    CreateInstance();
#if defined(FFV_DEBUG)
    CreateDebugCallback();
#endif
    CreateSurface(m_Window->GetNativeWindow());

    m_PhysicalDevices = MakeShared<PhysicalDevices>(m_Instance, m_Surface);
    m_QueueFamily = m_PhysicalDevices->SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);

    CreateDevice();

    m_Swapchain = MakeShared<Swapchain>(m_Device, m_PhysicalDevices, m_Surface, m_QueueFamily);

    std::vector<SharedPtr<Shader>> shaders = { MakeShared<Shader>(m_Device, "default.vert.spv"),
                                               MakeShared<Shader>(m_Device, "default.frag.spv") };
    m_GraphicsPipeline = MakeShared<GraphicsPipeline>(m_Device, shaders);

    CreateCommandBufferPool();
    CreateVertrexBuffer();
    CreateCommandBuffers(static_cast<U32>(m_Swapchain->GetImageViews().size()));
    RecordCommandBuffers();

    m_Queue = MakeShared<Queue>(m_Device, m_Swapchain->GetSwapchain(), m_QueueFamily, 0);
}

Renderer::~Renderer()
{
    m_Queue.reset();

    vkFreeCommandBuffers(m_Device, m_CommandBufferPool, m_CommandBuffers.size(), m_CommandBuffers.data());
    vkDestroyCommandPool(m_Device, m_CommandBufferPool, VK_NULL_HANDLE);

    vkDestroyBuffer(m_Device, m_VertexBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, m_VertexBufferMemory, VK_NULL_HANDLE);

    m_GraphicsPipeline.reset();
    m_Swapchain.reset();

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

void Renderer::Update()
{
    U32 imageIndex = m_Queue->AquireNextImage();
    m_Queue->SubmitAsync(m_CommandBuffers[imageIndex]);
    m_Queue->Present(imageIndex);
}

void Renderer::CreateInstance()
{
    const std::vector<const char*> layers = {
#if defined(FFV_DEBUG)
        "VK_LAYER_KHRONOS_validation", //"VK_LAYER_LUNARG_api_dump"
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

    FFV_ASSERT(m_PhysicalDevices->GetSelectedPhysicalDevice().Features.geometryShader != VK_FALSE,
               "Device does not support Geometry Shaders!", exit(1));
    FFV_ASSERT(m_PhysicalDevices->GetSelectedPhysicalDevice().Features.tessellationShader != VK_FALSE,
               "Device does not support Tessellation Shaders!", exit(1));

    VkPhysicalDeviceVulkan13Features vk13Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features vk12Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                                                      .pNext = &vk13Features };
    VkPhysicalDeviceVulkan11Features vk11Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
                                                      .pNext = &vk12Features };

    VkPhysicalDeviceFeatures2 deviceFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                                                 .pNext = &vk11Features };

    vkGetPhysicalDeviceFeatures2(m_PhysicalDevices->GetSelectedPhysicalDevice().PhysicalDevice, &deviceFeatures);

    const VkDeviceCreateInfo deviceCreateInfo = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                                  .pNext = &deviceFeatures,
                                                  .queueCreateInfoCount = 1,
                                                  .pQueueCreateInfos = &queueCreateInfo,
                                                  .enabledExtensionCount = static_cast<U32>(extensions.size()),
                                                  .ppEnabledExtensionNames = extensions.data() };

    FFV_CHECK_VK_RESULT(vkCreateDevice(m_PhysicalDevices->GetSelectedPhysicalDevice().PhysicalDevice, &deviceCreateInfo,
                                       VK_NULL_HANDLE, &m_Device));

    FFV_TRACE("Created vulkan device!");
}

void Renderer::CreateCommandBufferPool()
{
    const VkCommandPoolCreateInfo commandBufferCreateInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                              .queueFamilyIndex = m_QueueFamily };

    FFV_CHECK_VK_RESULT(vkCreateCommandPool(m_Device, &commandBufferCreateInfo, VK_NULL_HANDLE, &m_CommandBufferPool));
    FFV_TRACE("Created command pool!");
}

void Renderer::CreateVertrexBuffer()
{
    VkBufferCreateInfo bufferCreateInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                            .size = sizeof(m_Vertices[0]) * m_Vertices.size(),
                                            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE };

    vkCreateBuffer(m_Device, &bufferCreateInfo, VK_NULL_HANDLE, &m_VertexBuffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_Device, m_VertexBuffer, &memoryRequirements);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };

    FFV_CHECK_VK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, VK_NULL_HANDLE, &m_VertexBufferMemory));
    FFV_CHECK_VK_RESULT(vkBindBufferMemory(m_Device, m_VertexBuffer, m_VertexBufferMemory, 0));

    void* data;
    FFV_CHECK_VK_RESULT(vkMapMemory(m_Device, m_VertexBufferMemory, 0, memoryAllocateInfo.allocationSize, 0, &data));

    memcpy(data, m_Vertices.data(), memoryAllocateInfo.allocationSize);
    vkUnmapMemory(m_Device, m_VertexBufferMemory);

    FFV_TRACE("Created vertex buffer with {0} vertices!", m_Vertices.size());
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
    const std::vector<VkImageView>& imageViews = m_Swapchain->GetImageViews();
    const std::vector<VkImage>& images = m_Swapchain->GetImages();
    const U32 windowWidth = m_Window->GetWidth();
    const U32 windowHeight = m_Window->GetHeight();
    const VkClearValue clearColor = { .color = { .float32 = { 0.1f, 0.1f, 0.1f, 1.0f } } };
    const VkViewport viewport = { .width = static_cast<float>(windowWidth), .height = static_cast<float>(windowHeight) };
    const VkRect2D scissorRect = {
        .extent = { .width = windowWidth, .height = windowHeight }
    };
    const VkImageSubresourceRange imageSubResourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1
    };

    const VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                 .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT };

    for (U32 i = 0; i < m_CommandBuffers.size(); i++)
    {
        VkImageMemoryBarrier presentToClearBarrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                       .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                                                       .dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
                                                       .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                       .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .image = images[i],
                                                       .subresourceRange = imageSubResourceRange };

        VkImageMemoryBarrier clearToPresentBarrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                       .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                                       .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                                                       .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                       .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                       .image = images[i],
                                                       .subresourceRange = imageSubResourceRange };

        const VkRenderingAttachmentInfoKHR colorAttachment = { .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                                                               .imageView = imageViews[i],
                                                               .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                                               .resolveMode = VK_RESOLVE_MODE_NONE,
                                                               .resolveImageView = VK_NULL_HANDLE,
                                                               .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                               .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                               .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                               .clearValue = clearColor };

        const VkRenderingInfoKHR renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = { { 0, 0 }, { windowWidth, windowHeight } },
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment
        };

        FFV_CHECK_VK_RESULT(vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo));

        vkCmdPipelineBarrier(m_CommandBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                             VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &presentToClearBarrier);

        vkCmdBeginRendering(m_CommandBuffers[i], &renderingInfo);

        m_GraphicsPipeline->Bind(m_CommandBuffers[i]);

        vkCmdSetViewport(m_CommandBuffers[i], 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffers[i], 0, 1, &scissorRect);
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(m_CommandBuffers[i], 0, 1, &m_VertexBuffer, &offset);
        vkCmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRendering(m_CommandBuffers[i]);

        vkCmdPipelineBarrier(m_CommandBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0,
                             VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &clearToPresentBarrier);

        FFV_CHECK_VK_RESULT(vkEndCommandBuffer(m_CommandBuffers[i]));
    }

    FFV_TRACE("Command buffers have been recorded!");
}

U32 Renderer::FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevices->GetSelectedPhysicalDevice().PhysicalDevice, &memoryProperties);

    for (U32 i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    FFV_ASSERT(false, "Failed to find suitable memory type!", return 0);
}

} // namespace FFV
