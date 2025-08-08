#include "FastFileViewerPCH.h"

#include "GraphicsPipeline.h"

#include "renderer/Model.h"
#include "util/Log.h"
#include "util/Util.h"
#include "vulkan/vulkan_core.h"

#include <vector>

namespace FFV
{
GraphicsPipeline::GraphicsPipeline(VkDevice device, SharedPtr<Swapchain> swapchain,
                                   SharedPtr<PhysicalDevices> physicalDevice, SharedPtr<Window> window,
                                   std::vector<SharedPtr<Shader>> shaders)
    : m_Device(device), m_Swapchain(swapchain), m_PhysicalDevice(physicalDevice), m_Window(window)
{
    CreateDescriptorSetLayout();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    shaderStageCreateInfos.reserve(shaders.size());

    for (const auto& shaderStage : shaders)
    {
        shaderStageCreateInfos.emplace_back(
            VkPipelineShaderStageCreateInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                             .stage = shaderStage->GetShaderStage(),
                                             .module = shaderStage->GetShaderModule(),
                                             .pName = "main" });
    }

    VkVertexInputBindingDescription bindingDescription = Model::Vertex::GetBindingDescription();
    auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<U32>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { .sType =
                                                                    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                                                                .dynamicStateCount = static_cast<U32>(dynamicStates.size()),
                                                                .pDynamicStates = dynamicStates.data() };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, .viewportCount = 1, .scissorCount = 1
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE
    };

    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f
    };

    VkPipelineColorBlendAttachmentState colorBlendStateAttachment = { .blendEnable = VK_FALSE,
                                                                      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                                                        VK_COLOR_COMPONENT_G_BIT |
                                                                                        VK_COLOR_COMPONENT_B_BIT |
                                                                                        VK_COLOR_COMPONENT_A_BIT };

    VkPipelineColorBlendStateCreateInfo colorBlendeStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendStateAttachment
    };

    VkPipelineLayoutCreateInfo layoutCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                    .setLayoutCount = 1,
                                                    .pSetLayouts = &m_DescriptorSetLayout,
                                                    .pushConstantRangeCount = 0 };

    FFV_CHECK_VK_RESULT(vkCreatePipelineLayout(m_Device, &layoutCreateInfo, VK_NULL_HANDLE, &m_PipelineLayout));

    VkPipelineRenderingCreateInfo renderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                                                .pNext = &renderingCreateInfo,
                                                                .stageCount =
                                                                    static_cast<U32>(shaderStageCreateInfos.size()),
                                                                .pStages = shaderStageCreateInfos.data(),
                                                                .pVertexInputState = &vertexInputStateCreateInfo,
                                                                .pInputAssemblyState = &inputAssemblyCreateInfo,
                                                                .pViewportState = &viewportStateCreateInfo,
                                                                .pRasterizationState = &rasterizationStateCreateInfo,
                                                                .pMultisampleState = &multiSampleStateCreateInfo,
                                                                .pColorBlendState = &colorBlendeStateCreateInfo,

                                                                .pDynamicState = &dynamicStateCreateInfo,
                                                                .layout = m_PipelineLayout };

    FFV_CHECK_VK_RESULT(
        vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, VK_NULL_HANDLE, &m_Pipeline));

    FFV_TRACE("Created vulkan graphics pipeline!");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, VK_NULL_HANDLE);

    for (const auto& descriptorSet : m_DescriptorSets)
    {
        vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &descriptorSet);
    }

    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, VK_NULL_HANDLE);

    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, VK_NULL_HANDLE);
    vkDestroyPipeline(m_Device, m_Pipeline, VK_NULL_HANDLE);

    for (U32 i = 0; i < m_UniformBuffers.size(); i++)
    {
        vkDestroyBuffer(m_Device, m_UniformBuffers[i], VK_NULL_HANDLE);
        vkFreeMemory(m_Device, m_UniformBuffersMemory[i], VK_NULL_HANDLE);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer) const
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::UpdateUniformBuffer(U32 imageIndex)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo = {
        .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        .view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        .proj = glm::perspective(glm::radians(45.0f),
                                 static_cast<float>(std::max(m_Window->GetWidth(), 1u)) /
                                     static_cast<float>(std::max(m_Window->GetHeight(), 1u)),
                                 0.1f, 10.0f)
    };

    ubo.proj[1][1] *= -1.0f;

    std::memcpy(m_UniformBuffersMapped[imageIndex], &ubo, sizeof(ubo));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::CreateDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding uboLayoutBinding = { .binding = 0,
                                                            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                            .descriptorCount = 1,
                                                            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                                            .pImmutableSamplers = nullptr };

    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, .bindingCount = 1, .pBindings = &uboLayoutBinding
    };

    FFV_CHECK_VK_RESULT(
        vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, VK_NULL_HANDLE, &m_DescriptorSetLayout));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::CreateUniformBuffers()
{
    m_UniformBuffers.clear();
    m_UniformBuffersMemory.clear();
    m_UniformBuffersMapped.clear();

    for (U32 i = 0; i < m_Swapchain->GetNumImagesInFlight(); i++)
    {
        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        Util::CreateBuffer(m_Device, m_PhysicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer,
                           uniformBufferMemory);
        void* data;
        FFV_CHECK_VK_RESULT(vkMapMemory(m_Device, uniformBufferMemory, 0, bufferSize, 0, &data));
        m_UniformBuffersMapped.push_back(data);
        m_UniformBuffers.push_back(uniformBuffer);
        m_UniformBuffersMemory.push_back(uniformBufferMemory);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::CreateDescriptorPool()
{
    const VkDescriptorPoolSize poolSize = { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                            .descriptorCount = m_Swapchain->GetNumImagesInFlight() };

    const VkDescriptorPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = m_Swapchain->GetNumImagesInFlight(),
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    FFV_CHECK_VK_RESULT(vkCreateDescriptorPool(m_Device, &poolCreateInfo, VK_NULL_HANDLE, &m_DescriptorPool));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::CreateDescriptorSets()
{
    const std::vector<VkDescriptorSetLayout> layouts(m_Swapchain->GetNumImagesInFlight(), m_DescriptorSetLayout);
    const VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_DescriptorPool,
        .descriptorSetCount = static_cast<U32>(layouts.size()),
        .pSetLayouts = layouts.data(),
    };

    m_DescriptorSets.resize(layouts.size());
    FFV_CHECK_VK_RESULT(vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()));

    for (U32 i = 0; i < layouts.size(); i++)
    {
        const VkDescriptorBufferInfo bufferInfo = { .buffer = m_UniformBuffers[i],
                                                    .offset = 0,
                                                    .range = sizeof(UniformBufferObject) };

        const VkWriteDescriptorSet descriptorWrite = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                                       .dstSet = m_DescriptorSets[i],
                                                       .dstBinding = 0,
                                                       .dstArrayElement = 0,
                                                       .descriptorCount = 1,
                                                       .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                       .pBufferInfo = &bufferInfo };

        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
}
} // namespace FFV
