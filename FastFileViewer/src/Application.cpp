#include "Application.h"

#include <iostream>
#include <array>

namespace FFV
{
    Application::Application()
    {
        LoadModels();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    Application::~Application()
    {
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void Application::Run()
    {
        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(m_Device.GetDevice());
    }

    void Application::LoadModels()
    {
        std::vector<Model::Vertex> vertices
        {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        m_Model = std::make_unique<Model>(m_Device, vertices);
    }

    void Application::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            std::cerr << "Failed to create pipeline layout" << std::endl;
        }
    }

    void Application::CreatePipeline()
    {
        PipelineConfigInfo pipelineConfig = Pipeline::DefaultPipelineConfigInfo(m_Swapchain.Width(), m_Swapchain.Height());
        pipelineConfig.renderPass = m_Swapchain.GetRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        m_Pipeline = std::make_unique<Pipeline>(m_Device, "assets/shaders/Default.vert", "assets/shaders/Default.frag", pipelineConfig);
    }

    void Application::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_Swapchain.ImageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device.GetCommandPool();
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            std::cerr << "Failed to allocate command buffers" << std::endl;
        }

        for (int i = 0; i < m_CommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                std::cerr << "Failed to begin recording command buffer" << std::endl;
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_Swapchain.GetRenderPass();
            renderPassInfo.framebuffer = m_Swapchain.GetFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = m_Swapchain.GetSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_Pipeline->Bind(m_CommandBuffers[i]);
            m_Model->Bind(m_CommandBuffers[i]);
            m_Model->Draw(m_CommandBuffers[i]);

            vkCmdEndRenderPass(m_CommandBuffers[i]);

            if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
            {
                std::cerr << "Failed to record command buffer" << std::endl;
            }
        }
    }

    void Application::DrawFrame()
    {
        uint32_t imageIndex;
        VkResult result = m_Swapchain.AcquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            std::cerr << "Failed to acquire swap chain image" << std::endl;
        }

        result = m_Swapchain.SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

        if (result != VK_SUCCESS)
        {
            std::cerr << "Failed to present swap chain image" << std::endl;
        }
    }
}
