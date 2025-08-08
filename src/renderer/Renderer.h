#pragma once

#include "Model.h"
#include "Window.h"
#include "renderer/GraphicsPipeline.h"
#include "renderer/PhysicalDevice.h"
#include "renderer/Queue.h"
#include "renderer/Swapchain.h"
#include "util/Types.h"
#include "util/Util.h"

#include <GLFW/glfw3.h>

namespace FFV
{
class Renderer
{
public:
    Renderer(SharedPtr<Window> window);
    ~Renderer();

    FFV_DELETE_MOVE_COPY(Renderer);

    void Update();
    void WaitIdle() const { vkDeviceWaitIdle(m_Device); }

private:
    void CreateInstance();
    void CreateDebugCallback();
    void CreateSurface(GLFWwindow* window);
    void CreateDevice();
    void CreateCommandBufferPool();
    void CreateCommandBuffers(U32 count);
    void RecordCommandBuffer(U32 imageIndex);

    void CreateImageBarrier(U32 imageIndex, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags2 srcAccessMask,
                            VkAccessFlags2 dstAccessMask, VkPipelineStageFlags2 srcStageMask,
                            VkPipelineStageFlags2 dstStageMask);

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkCommandPool m_CommandBufferPool = VK_NULL_HANDLE;

    SharedPtr<Window> m_Window;
    SharedPtr<PhysicalDevices> m_PhysicalDevices;
    SharedPtr<Swapchain> m_Swapchain;
    SharedPtr<Queue> m_Queue;
    SharedPtr<GraphicsPipeline> m_GraphicsPipeline;

    U32 m_QueueFamily = 0;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    // Tmp
    SharedPtr<Model> m_Model;

    const std::vector<Model::Vertex> m_Vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f },  { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f },   { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f },  { 1.0f, 1.0f, 1.0f } }
    };

    const std::vector<U32> m_Indices = { 0, 3, 2, 2, 1, 0 };
};
} // namespace FFV
