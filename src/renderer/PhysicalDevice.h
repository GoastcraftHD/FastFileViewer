#pragma once

#include "util/Types.h"
#include "util/Util.h"

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
    PhysicalDevices() = default;
    PhysicalDevices(const VkInstance& instance, const VkSurfaceKHR& surface);
    ~PhysicalDevices();

    FFV_DELETE_COPY(PhysicalDevices);

    U32 SelectDevice(VkQueueFlags requiredQueueType, bool supportsPresent);
    const PhysicalDevice& GetSelectedPhysicalDevice() const;

private:
    std::vector<PhysicalDevice> m_PhysicalDevices;
    I32 m_SelectedDeviceIndex = -1;
};
} // namespace FFV
