#include "Renderer.h"

#include "FastFileViewerPCH.h"
#include "util/Log.h"
#include "util/Util.h"
#include "vulkan/vulkan_core.h"

namespace FFV
{
Renderer::Renderer()
{
    CreateInstance();
}

Renderer::~Renderer()
{
    vkDestroyInstance(m_Instance, nullptr);
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

    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .pApplicationName = "Fast File Viewer",
                                  .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                                  .pEngineName = "Fast",
                                  .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
                                  .apiVersion = VK_API_VERSION_1_0 };

    VkInstanceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                        .pApplicationInfo = &appInfo,
                                        .enabledLayerCount = static_cast<U32>(layers.size()),
                                        .ppEnabledLayerNames = layers.data(),
                                        .enabledExtensionCount =
                                            static_cast<U32>(extensions.size()),
                                        .ppEnabledExtensionNames = extensions.data() };

    FFV_CHECK_VK_RESULT(vkCreateInstance(&createInfo, nullptr, &m_Instance));
    FFV_TRACE("Created vulkan instance!");
}

} // namespace FFV
