#pragma once

#include "util/Types.h"
#include "vulkan/vulkan_core.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace FFV
{
struct PhysicalDevice
{
    VkPhysicalDevice PhysicalDevice;
    VkPhysicalDeviceProperties DeviceProperties;
    std::vector<VkQueueFamilyProperties> QueueFamiliyProperties;
    std::vector<VkBool32> QueueSupportsPresent;
    std::vector<VkSurfaceFormatKHR> SurfaceFormats;
    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    std::vector<VkPresentModeKHR> PresentModes;
    VkPhysicalDeviceFeatures Features;
};

class PhysicalDevices
{
public:
    PhysicalDevices() {}
    PhysicalDevices(const VkInstance& instance, const VkSurfaceKHR& surface);
    ~PhysicalDevices();

    U32 SelectDevice(VkQueueFlags requiredQueueType, bool supportsPresent);
    const PhysicalDevice& GetSelectedPhysicalDevice() const;

private:
    std::vector<PhysicalDevice> m_PhysicalDevices;
    int m_SelectedDeviceIndex = -1;
};
} // namespace FFV
