#pragma once

#include "renderer/Shader.h"
#include "util/Types.h"
#include "util/Util.h"

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace FFV
{
class GraphicsPipeline
{
public:
    struct Vertex
    {
        glm::vec2 position;
        glm::vec3 color;

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            return { .binding = 0, .stride = sizeof(Vertex), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };
        }

        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
        {
            return {
                VkVertexInputAttributeDescription{
                                                  .location = 0, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,    .offset = offsetof(Vertex, position) },
                VkVertexInputAttributeDescription{
                                                  .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, color)    }
            };
        }
    };

public:
    GraphicsPipeline(VkDevice device, std::vector<SharedPtr<Shader>> shaders);
    ~GraphicsPipeline();

    FFV_DELETE_MOVE_COPY(GraphicsPipeline);

    void Bind(VkCommandBuffer commandBuffer) const;

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
};
} // namespace FFV
