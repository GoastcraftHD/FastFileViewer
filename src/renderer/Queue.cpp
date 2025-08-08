#include "FastFileViewerPCH.h"

#include "Queue.h"

#include "vulkan/vulkan_core.h"

namespace FFV
{
Queue::Queue(VkDevice device, SharedPtr<Swapchain> swapchain, U32 queueFamily, U32 queueIndex)
    : m_Device(device), m_Swapchain(swapchain)
{
    vkGetDeviceQueue(m_Device, queueFamily, queueIndex, &m_Queue);
    FFV_TRACE("Queue acquired!");
    CreateSyncObjects();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Queue::~Queue()
{
    for (U32 i = 0; i < m_Swapchain->GetNumImagesInFlight(); i++)
    {
        vkDestroySemaphore(m_Device, m_RenderCompleteSemaphores[i], VK_NULL_HANDLE);
        vkDestroySemaphore(m_Device, m_PresentCompleteSemaphores[i], VK_NULL_HANDLE);
        vkDestroyFence(m_Device, m_InFlightFences[i], VK_NULL_HANDLE);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U32 Queue::AquireNextImage() const
{
    FFV_CHECK_VK_RESULT(
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, std::numeric_limits<U64>::max()));

    U32 imageIndex = 0;
    const VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain->GetSwapchain(), std::numeric_limits<U64>::max(),
                                                  m_PresentCompleteSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_Swapchain->Recreate();
        return AquireNextImage(); // Retry acquiring the next image after swapchain recreation
    }
    else
    {
        FFV_CHECK_VK_RESULT(result);
    }

    FFV_CHECK_VK_RESULT(vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]));

    return imageIndex;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Queue::Submit(VkCommandBuffer commandBuffer) const
{
    const VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .commandBufferCount = 1,
                                      .pCommandBuffers = &commandBuffer };

    FFV_CHECK_VK_RESULT(vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Queue::SubmitAsync(VkCommandBuffer commandBuffer, U32 imageIndex) const
{
    const VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    const VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .waitSemaphoreCount = 1,
                                      .pWaitSemaphores = &m_PresentCompleteSemaphores[m_CurrentFrame],
                                      .pWaitDstStageMask = &waitFlags,
                                      .commandBufferCount = 1,
                                      .pCommandBuffers = &commandBuffer,
                                      .signalSemaphoreCount = 1,
                                      .pSignalSemaphores = &m_RenderCompleteSemaphores[imageIndex] };

    FFV_CHECK_VK_RESULT(vkQueueSubmit(m_Queue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Queue::Present(U32 imageIndex)
{
    FFV_ASSERT(imageIndex < m_Swapchain->GetNumImagesInFlight(),
               std::format("Image index {} is out of bounds! Swapchain images in flight: {}", imageIndex,
                           m_Swapchain->GetNumImagesInFlight()),
               return);

    const VkPresentInfoKHR presentInfo = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                           .waitSemaphoreCount = 1,
                                           .pWaitSemaphores = &m_RenderCompleteSemaphores[imageIndex],
                                           .swapchainCount = 1,
                                           .pSwapchains = &m_Swapchain->GetSwapchain(),
                                           .pImageIndices = &imageIndex };

    const VkResult result = vkQueuePresentKHR(m_Queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        m_Swapchain->Recreate();
        return;
    }
    else
    {
        FFV_CHECK_VK_RESULT(result);
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % m_Swapchain->GetNumImagesInFlight();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Queue::CreateSyncObjects()
{
    m_PresentCompleteSemaphores.clear();
    m_RenderCompleteSemaphores.clear();
    m_InFlightFences.clear();

    const VkSemaphoreCreateInfo semaphoreCreateInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    const VkFenceCreateInfo fenceCreateInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                .flags = VK_FENCE_CREATE_SIGNALED_BIT };

    for (U32 i = 0; i < m_Swapchain->GetNumImagesInFlight(); i++)
    {
        VkSemaphore renderCompleteSemaphore;
        FFV_CHECK_VK_RESULT(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, VK_NULL_HANDLE, &renderCompleteSemaphore));
        m_RenderCompleteSemaphores.push_back(renderCompleteSemaphore);

        VkSemaphore presentCompleteSemaphore;
        FFV_CHECK_VK_RESULT(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, VK_NULL_HANDLE, &presentCompleteSemaphore));
        m_PresentCompleteSemaphores.push_back(presentCompleteSemaphore);

        VkFence inFlightFence;
        FFV_CHECK_VK_RESULT(vkCreateFence(m_Device, &fenceCreateInfo, VK_NULL_HANDLE, &inFlightFence));
        m_InFlightFences.push_back(inFlightFence);
    }
}
} // namespace FFV
