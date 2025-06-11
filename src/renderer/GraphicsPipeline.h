#pragma once

#include "renderer/Shader.h"
#include "util/Types.h"
#include "util/Util.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace FFV
{
class GraphicsPipeline
{
public:
    GraphicsPipeline(VkDevice device, std::vector<SharedPtr<Shader>> shaders);
    ~GraphicsPipeline();

    FFV_DELETE_MOVE_COPY(GraphicsPipeline);

    void Bind(VkCommandBuffer commandBuffer);

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
};
} // namespace FFV
