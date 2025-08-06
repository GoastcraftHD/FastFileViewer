#pragma once

#include "renderer/PhysicalDevice.h"
#include "renderer/Shader.h"
#include "renderer/Swapchain.h"
#include "util/Types.h"
#include "util/Util.h"

#include <Window.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace FFV
{
class GraphicsPipeline
{
public:
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

public:
    GraphicsPipeline(VkDevice device, SharedPtr<Swapchain> swapchain, SharedPtr<PhysicalDevices> physicalDevice,
                     SharedPtr<Window> window, std::vector<SharedPtr<Shader>> shaders);
    ~GraphicsPipeline();

    FFV_DELETE_MOVE_COPY(GraphicsPipeline);

    void Bind(VkCommandBuffer commandBuffer) const;
    void UpdateUniformBuffer(U32 imageIndex);

    const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }
    VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

private:
    void CreateDescriptorSetLayout();
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    SharedPtr<Swapchain> m_Swapchain;
    SharedPtr<PhysicalDevices> m_PhysicalDevice;
    SharedPtr<Window> m_Window;

    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_DescriptorSets;

    std::vector<VkBuffer> m_UniformBuffers;
    std::vector<VkDeviceMemory> m_UniformBuffersMemory;
    std::vector<void*> m_UniformBuffersMapped;
};
} // namespace FFV
