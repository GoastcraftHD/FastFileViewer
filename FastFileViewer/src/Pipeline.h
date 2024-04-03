#pragma once

#include "Device.h"

#include <string>
#include <vector>

namespace FFV
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline
    {
    public:
        Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);
        ~Pipeline();

        void Bind(VkCommandBuffer commandBuffer);

        static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);

    private:
        static std::string ReadFile(const std::string& path);

        void CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);
        void CreateShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule);

        Device& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
