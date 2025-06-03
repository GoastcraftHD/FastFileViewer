#include "FastFileViewerPCH.h"

#include "Queue.h"

#include "vulkan/vulkan_core.h"

namespace FFV
{

Queue::Queue(VkDevice device, VkSwapchainKHR swapchain, U32 queueFamily, U32 queueIndex)
    : m_Device(device), m_Swapchain(swapchain)
{
    vkGetDeviceQueue(m_Device, queueFamily, queueIndex, &m_Queue);
    FFV_TRACE("Queue acquired!");
    CreateSyncObjects();
}

Queue::~Queue()
{
    vkDestroySemaphore(m_Device, m_RenderCompleteSemaphore, VK_NULL_HANDLE);
    vkDestroySemaphore(m_Device, m_PresentCompleteSemaphore, VK_NULL_HANDLE);
}

U32 Queue::AquireNextImage()
{
    U32 imageIndex = 0;
    FFV_CHECK_VK_RESULT(vkAcquireNextImageKHR(m_Device, m_Swapchain, std::numeric_limits<U64>::max(),
                                              m_PresentCompleteSemaphore, VK_NULL_HANDLE, &imageIndex));

    return imageIndex;
}

void Queue::Submit(VkCommandBuffer commandBuffer)
{
    const VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .commandBufferCount = 1,
                                      .pCommandBuffers = &commandBuffer };

    FFV_CHECK_VK_RESULT(vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE));
}

void Queue::SubmitAsync(VkCommandBuffer commandBuffer)
{
    const VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    const VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .waitSemaphoreCount = 1,
                                      .pWaitSemaphores = &m_PresentCompleteSemaphore,
                                      .pWaitDstStageMask = &waitFlags,
                                      .commandBufferCount = 1,
                                      .pCommandBuffers = &commandBuffer,
                                      .signalSemaphoreCount = 1,
                                      .pSignalSemaphores = &m_RenderCompleteSemaphore };

    FFV_CHECK_VK_RESULT(vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE));
}

void Queue::Present(U32 imageIndex)
{
    const VkPresentInfoKHR presentInfo = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                           .waitSemaphoreCount = 1,
                                           .pWaitSemaphores = &m_RenderCompleteSemaphore,
                                           .swapchainCount = 1,
                                           .pSwapchains = &m_Swapchain,
                                           .pImageIndices = &imageIndex };

    FFV_CHECK_VK_RESULT(vkQueuePresentKHR(m_Queue, &presentInfo));
    WaitIdle();
}

void Queue::WaitIdle() { vkQueueWaitIdle(m_Queue); }

void Queue::CreateSyncObjects()
{
    const VkSemaphoreCreateInfo semaphoreCreateInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    FFV_CHECK_VK_RESULT(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, VK_NULL_HANDLE, &m_RenderCompleteSemaphore));
    FFV_CHECK_VK_RESULT(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, VK_NULL_HANDLE, &m_PresentCompleteSemaphore));
}
} // namespace FFV
