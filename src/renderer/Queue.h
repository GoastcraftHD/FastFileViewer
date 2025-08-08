#pragma once

#include "renderer/Swapchain.h"
#include "util/Types.h"
#include "util/Util.h"

#include <vulkan/vulkan.h>

namespace FFV
{
class Queue
{
public:
    Queue() = default;
    Queue(VkDevice device, SharedPtr<Swapchain> swapchain, U32 queueFamily, U32 queueIndex);
    ~Queue();

    FFV_DELETE_MOVE_COPY(Queue);

    U32 AquireNextImage() const;
    void Submit(VkCommandBuffer commandBuffer) const;
    void SubmitAsync(VkCommandBuffer commandBuffer, U32 imageIndex) const;
    void Present(U32 imageIndex);
    void WaitIdle() const { vkQueueWaitIdle(m_Queue); }
    const VkQueue& GetQueue() const { return m_Queue; }

private:
    void CreateSyncObjects();

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    SharedPtr<Swapchain> m_Swapchain;

    VkQueue m_Queue = VK_NULL_HANDLE;
    std::vector<VkSemaphore> m_RenderCompleteSemaphores;
    std::vector<VkSemaphore> m_PresentCompleteSemaphores;
    std::vector<VkFence> m_InFlightFences;

    U32 m_CurrentFrame = 0;
};
} // namespace FFV
