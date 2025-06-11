#pragma once

#include "renderer/GraphicsPipeline.h"
struct GLFWwindow;

#include "Window.h"
#include "renderer/PhysicalDevice.h"
#include "renderer/Queue.h"
#include "renderer/Swapchain.h"
#include "util/Types.h"
#include "util/Util.h"

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
    void CreateCommandBuffers(U32 count);
    void RecordCommandBuffers();

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
};
} // namespace FFV
