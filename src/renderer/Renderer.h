#pragma once

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

private:
    void CreateInstance();
    void CreateDebugCallback();
    void CreateSurface(GLFWwindow* window);
    void CreateDevice();
    void CreateCommandBufferPool();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateCommandBuffers(U32 count);
    void RecordCommandBuffers();

    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) const;
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

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

    VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;

    // Tmp
    const std::vector<GraphicsPipeline::Vertex> m_Vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f },  { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f },   { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f },  { 1.0f, 1.0f, 1.0f } }
    };

    const std::vector<U32> m_Indices = { 0, 3, 2, 2, 1, 0 };
};
} // namespace FFV
