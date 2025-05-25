#include "Renderer.h"

#include "FastFileViewerPCH.h"
#include "util/Log.h"
#include "util/Util.h"
#include "vulkan/vk_platform.h"
#include "vulkan/vulkan_core.h"

namespace FFV
{
Renderer::Renderer()
{
    CreateInstance();
    CreateDebugCallback();
}

Renderer::~Renderer()
{
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
    FFV_ASSERT(vkDestroyDebugUtilsMessenger, "Cannot find address of vkDestroyDebugUtilsMessenger",
               ;);
    vkDestroyDebugUtilsMessenger(m_Instance, m_DebugMessenger, VK_NULL_HANDLE);
    vkDestroyInstance(m_Instance, VK_NULL_HANDLE);
}

void Renderer::CreateInstance()
{
    std::array<const char*, 1> layers = { "VK_LAYER_KHRONOS_validation" };
    std::array<const char*, 4> extensions = { VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(FFV_LINUX)
                                              "VK_KHR_xcb_surface",
#elif defined(FFV_WINDOWS)
                                              "VK_KHR_win32_surface",
#endif
                                              VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                                              VK_EXT_DEBUG_REPORT_EXTENSION_NAME };

    const VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                        .pApplicationName = "Fast File Viewer",
                                        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                                        .pEngineName = "Fast",
                                        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                                        .apiVersion = VK_API_VERSION_1_0 };

    const VkInstanceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                              .pApplicationInfo = &appInfo,
                                              .enabledLayerCount = static_cast<U32>(layers.size()),
                                              .ppEnabledLayerNames = layers.data(),
                                              .enabledExtensionCount =
                                                  static_cast<U32>(extensions.size()),
                                              .ppEnabledExtensionNames = extensions.data() };

    FFV_CHECK_VK_RESULT(vkCreateInstance(&createInfo, VK_NULL_HANDLE, &m_Instance));
    FFV_TRACE("Created vulkan instance!");
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
              const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
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
    "                Type: {0}\n"    \
    "                Objects: {1}\n" \
    "                Message: {2}"

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
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &DebugCallback
    };

    const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
    FFV_ASSERT(vkCreateDebugUtilsMessenger, "Cannot find address of vkCreateDebugUtilsMessenger",
               return);
    FFV_CHECK_VK_RESULT(vkCreateDebugUtilsMessenger(m_Instance, &messengerCreateInfo,
                                                    VK_NULL_HANDLE, &m_DebugMessenger));
    FFV_TRACE("Created vulkan debug callback!");
}

} // namespace FFV
