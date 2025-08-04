#pragma once

#include "PhysicalDevice.h"
#include "Queue.h"
#include "renderer/GraphicsPipeline.h"
#include "util/Types.h"
#include "util/Util.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace FFV
{
class Model
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
    Model(const std::vector<Vertex>& vertices, const std::vector<U32>& indices, VkDevice device,
          SharedPtr<PhysicalDevices> physicalDevice, SharedPtr<Queue> queue, VkCommandPool commandBufferPool);
    ~Model();

    FFV_DELETE_MOVE_COPY(Model);

    VkBuffer GetVertexBuffer() const { return m_VertexBuffer; }
    VkBuffer GetIndexBuffer() const { return m_IndexBuffer; }

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<U32>& indices);

    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) const;
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkCommandPool m_CommandBufferPool = VK_NULL_HANDLE;
    SharedPtr<PhysicalDevices> m_PhysicalDevices;
    SharedPtr<Queue> m_Queue;

    VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;
};
} // namespace FFV
