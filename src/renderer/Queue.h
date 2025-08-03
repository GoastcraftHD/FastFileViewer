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

    FFV_DELETE_MOVE_COPY(Queue);

    U32 AquireNextImage() const;
    void Submit(VkCommandBuffer commandBuffer) const;
    void SubmitAsync(VkCommandBuffer commandBuffer) const;
    void Present(U32 imageIndex) const;
    void WaitIdle() const;
    const VkQueue& GetQueue() const { return m_Queue; }

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
