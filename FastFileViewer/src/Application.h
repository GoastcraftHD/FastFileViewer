#pragma once

#include "Window.h"
#include "Pipeline.h"
#include "Device.h"
#include "Swapchain.h"
#include "Model.h"

#include <memory>
#include <vector>

namespace FFV
{
    class Application
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();

        void Run();

    private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();

        Window m_Window{ WIDTH, HEIGHT, "Hello Vulkan!" };
        Device m_Device{m_Window};
        Swapchain m_Swapchain{ m_Device, m_Window.GetExtent() };
        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::unique_ptr<Model> m_Model;
    };
}
