#pragma once

#include "util/Types.h"
#include "util/Util.h"

#include <vulkan/vulkan.h>

namespace FFV
{
class Queue
{
public:
    Queue() = default;
    Queue(VkDevice device, VkSwapchainKHR swapchain, U32 queueFamily, U32 queueIndex);
    ~Queue();

    U32 AquireNextImage();
    void Submit(VkCommandBuffer commandBuffer);
    void SubmitAsync(VkCommandBuffer commandBuffer);
    void Present(U32 imageIndex);
    void WaitIdle();

private:
    void CreateSyncObjects();

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkQueue m_Queue = VK_NULL_HANDLE;
    VkSemaphore m_RenderCompleteSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_PresentCompleteSemaphore = VK_NULL_HANDLE;
};
} // namespace FFV
