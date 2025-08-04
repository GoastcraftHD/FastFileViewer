#include "FastFileViewerPCH.h"

#include "GraphicsPipeline.h"

#include "util/Log.h"
#include "util/Util.h"
#include "vulkan/vulkan_core.h"

#include <vector>

namespace FFV
{
GraphicsPipeline::GraphicsPipeline(VkDevice device, std::vector<SharedPtr<Shader>> shaders) : m_Device(device)
{
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

    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

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
        .cullMode = VK_CULL_MODE_BACK_BIT,
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

    VkPipelineLayoutCreateInfo layoutCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

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
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, VK_NULL_HANDLE);
    vkDestroyPipeline(m_Device, m_Pipeline, VK_NULL_HANDLE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer) const
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
}
} // namespace FFV
