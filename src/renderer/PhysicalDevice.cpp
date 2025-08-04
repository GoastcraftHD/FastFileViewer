#include "FastFileViewerPCH.h"

#include "PhysicalDevice.h"

#include "vulkan/vulkan_core.h"

namespace FFV
{
PhysicalDevices::PhysicalDevices(const VkInstance& instance, const VkSurfaceKHR& surface)
{
    U32 numDevices = 0;
    FFV_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(instance, &numDevices, VK_NULL_HANDLE));

    FFV_TRACE("Number of Physical devices: {0}", numDevices);

    m_PhysicalDevices.resize(numDevices);

    std::vector<VkPhysicalDevice> devices;
    devices.resize(numDevices);

    FFV_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(instance, &numDevices, devices.data()));

    for (U32 i = 0; i < numDevices; i++)
    {
        const VkPhysicalDevice device = devices[i];
        m_PhysicalDevices[i].PhysicalDevice = device;

        vkGetPhysicalDeviceProperties(device, &m_PhysicalDevices[i].DeviceProperties);

        U32 numQueueFamilies = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, VK_NULL_HANDLE);
        m_PhysicalDevices[i].QueueFamiliyProperties.resize(numQueueFamilies);
        m_PhysicalDevices[i].QueueSupportsPresent.resize(numQueueFamilies);

        vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies,
                                                 m_PhysicalDevices[i].QueueFamiliyProperties.data());

        U32 apiVersion = m_PhysicalDevices[i].DeviceProperties.apiVersion;
        std::stringstream queueFamilyDebugSs;
        for (U32 j = 0; j < numQueueFamilies; j++)
        {
            const VkQueueFamilyProperties& queueFamiliyProperties = m_PhysicalDevices[i].QueueFamiliyProperties[j];

            queueFamilyDebugSs << "                " << j << ":\n";
            queueFamilyDebugSs << "                  GFX: "
                               << ((queueFamiliyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "True" : "False") << "\n";
            queueFamilyDebugSs << "                  Compute: "
                               << ((queueFamiliyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) ? "True" : "False") << "\n";
            queueFamilyDebugSs << "                  Transfer: "
                               << ((queueFamiliyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) ? "True" : "False") << "\n";
            queueFamilyDebugSs << "                  Sparse binding: "
                               << ((queueFamiliyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "True" : "False")
                               << "\n";

            FFV_CHECK_VK_RESULT(
                vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &(m_PhysicalDevices[i].QueueSupportsPresent[j])));
        }

        U32 numFormats = 0;
        FFV_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, VK_NULL_HANDLE));

        FFV_ASSERT(numFormats > 0, "", exit(1));

        m_PhysicalDevices[i].SurfaceFormats.resize(numFormats);

        FFV_CHECK_VK_RESULT(
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, m_PhysicalDevices[i].SurfaceFormats.data()));

        std::stringstream formatsDebugSs;
        for (U32 j = 0; j < numFormats; j++)
        {
            const VkSurfaceFormatKHR& surfaceFormat = m_PhysicalDevices[i].SurfaceFormats[j];
            formatsDebugSs << "                Surface " << j << ":\n                  Format: " << surfaceFormat.format
                           << "\n                  Color Space: " << surfaceFormat.colorSpace << "\n";
        }

        FFV_CHECK_VK_RESULT(
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &(m_PhysicalDevices[i].SurfaceCapabilities)));
        const VkImageUsageFlags& imageUsageFlags = m_PhysicalDevices[i].SurfaceCapabilities.supportedUsageFlags;

        std::stringstream imageUsageFlagsDebugSs;
        imageUsageFlagsDebugSs << "                Transfer src: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) ? "True" : "False");
        imageUsageFlagsDebugSs << "\n                Transfer dst: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) ? "True" : "False");
        imageUsageFlagsDebugSs << "\n                Sampled: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) ? "True" : "False");
        imageUsageFlagsDebugSs << "\n                Color attachment: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) ? "True" : "False");
        imageUsageFlagsDebugSs << "\n                Depth stencil attachment: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ? "True" : "False");
        imageUsageFlagsDebugSs << "\n                Transient attachment: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) ? "True" : "False");
        imageUsageFlagsDebugSs << "\n                Input attachment: "
                               << ((imageUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) ? "True" : "False") << "\n";

        U32 numPresentModes = 0;
        FFV_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numPresentModes, VK_NULL_HANDLE));

        FFV_ASSERT(numPresentModes != 0, "", exit(1));

        m_PhysicalDevices[i].PresentModes.resize(numPresentModes);

        FFV_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numPresentModes,
                                                                      m_PhysicalDevices[i].PresentModes.data()));

        vkGetPhysicalDeviceMemoryProperties(device, &(m_PhysicalDevices[i].MemoryProperties));

        std::stringstream memoryTypeDebugSs;
        for (U32 j = 0; j < m_PhysicalDevices[i].MemoryProperties.memoryTypeCount; j++)
        {
            memoryTypeDebugSs << "                " << j << ":\n";
            memoryTypeDebugSs << "                  Flags: "
                              << m_PhysicalDevices[i].MemoryProperties.memoryTypes[j].propertyFlags;
            memoryTypeDebugSs << " Heap: " << m_PhysicalDevices[i].MemoryProperties.memoryTypes[j].heapIndex << "\n";
        }

        vkGetPhysicalDeviceFeatures(m_PhysicalDevices[i].PhysicalDevice, &m_PhysicalDevices[i].Features);

        std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                               VK_FORMAT_D24_UNORM_S8_UINT };
        for (const auto& format : depthFormats)
        {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(device, format, &properties);

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                m_PhysicalDevices[i].DepthFormat = format;
                break;
            }
        }

        FFV_TRACE("Device Index: {0}\n"
                  "                Device name: {1}\n"
                  "                API version: {2}.{3}.{4}.{5}\n"
                  "                Number of Queue Families: {6}\n"
                  "{7}"
                  "{8}"
                  "{9}"
                  "                Number of Present Modes: {10}\n"
                  "                Number of Memory Types: {11}\n"
                  "{12}"
                  "                Number of Heap Types: {13}\n",
                  i, m_PhysicalDevices[i].DeviceProperties.deviceName, VK_API_VERSION_VARIANT(apiVersion),
                  VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion), VK_API_VERSION_PATCH(apiVersion),
                  numQueueFamilies, queueFamilyDebugSs.str(), formatsDebugSs.str(), imageUsageFlagsDebugSs.str(),
                  numPresentModes, m_PhysicalDevices[i].MemoryProperties.memoryTypeCount, memoryTypeDebugSs.str(),
                  m_PhysicalDevices[i].MemoryProperties.memoryHeapCount);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U32 PhysicalDevices::SelectDevice(VkQueueFlags requiredQueueType, bool supportsPresent)
{
    for (U32 i = 0; i < m_PhysicalDevices.size(); i++)
    {
        for (U32 j = 0; j < m_PhysicalDevices[i].QueueFamiliyProperties.size(); j++)
        {
            const VkQueueFamilyProperties& queueFamilyProperty = m_PhysicalDevices[i].QueueFamiliyProperties[j];

            if ((queueFamilyProperty.queueFlags & requiredQueueType) &&
                (static_cast<bool>(m_PhysicalDevices[i].QueueSupportsPresent[j]) == supportsPresent))
            {
                m_SelectedDeviceIndex = i;
                FFV_LOG("Using GFX device {0} and queue family {1}", i, j);
                return j;
            }
        }
    }

    FFV_ASSERT(false, "No suitable GFX device was found", ;);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const PhysicalDevice& PhysicalDevices::GetSelectedPhysicalDevice() const
{
    FFV_ASSERT(m_SelectedDeviceIndex >= 0 && m_SelectedDeviceIndex < m_PhysicalDevices.size(), "Invalid device index!", ;);
    return m_PhysicalDevices[m_SelectedDeviceIndex];
}
} // namespace FFV
