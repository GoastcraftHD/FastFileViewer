#include "FastFileViewerPCH.h"

#include "Model.h"

namespace FFV
{
Model::Model(const std::vector<GraphicsPipeline::Vertex>& vertices, const std::vector<U32>& indices, VkDevice device,
             SharedPtr<PhysicalDevices> physicalDevice, SharedPtr<Queue> queue, VkCommandPool commandBufferPool)
    : m_Device(device), m_PhysicalDevices(physicalDevice), m_Queue(queue), m_CommandBufferPool(commandBufferPool)
{
    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Model::~Model()
{
    vkDestroyBuffer(m_Device, m_VertexBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, m_VertexBufferMemory, VK_NULL_HANDLE);
    vkDestroyBuffer(m_Device, m_IndexBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, m_IndexBufferMemory, VK_NULL_HANDLE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::CreateVertexBuffer(const std::vector<GraphicsPipeline::Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                 stagingBufferMemory);

    void* dataStaging;
    FFV_CHECK_VK_RESULT(vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &dataStaging));

    memcpy(dataStaging, vertices.data(), bufferSize);
    vkUnmapMemory(m_Device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

    CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, stagingBufferMemory, VK_NULL_HANDLE);

    FFV_TRACE("Created vertex buffer with {0} vertices!", vertices.size());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::CreateIndexBuffer(const std::vector<U32>& indices)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                 stagingBufferMemory);

    void* dataStaging;
    FFV_CHECK_VK_RESULT(vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &dataStaging));

    memcpy(dataStaging, indices.data(), bufferSize);
    vkUnmapMemory(m_Device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

    CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, stagingBufferMemory, VK_NULL_HANDLE);

    FFV_TRACE("Created index buffer with {0} indicies!", indices.size());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U32 Model::FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevices->GetSelectedPhysicalDevice().PhysicalDevice, &memoryProperties);

    for (U32 i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    FFV_ASSERT(false, "Failed to find suitable memory type!", return 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags,
                         VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferCreateInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                            .size = size,
                                            .usage = usageFlags,
                                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE };

    vkCreateBuffer(m_Device, &bufferCreateInfo, VK_NULL_HANDLE, &buffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_Device, buffer, &memoryRequirements);
    VkMemoryAllocateInfo memoryAllocateInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                .allocationSize = memoryRequirements.size,
                                                .memoryTypeIndex =
                                                    FindMemoryType(memoryRequirements.memoryTypeBits, propertyFlags) };

    FFV_CHECK_VK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, VK_NULL_HANDLE, &bufferMemory));
    FFV_CHECK_VK_RESULT(vkBindBufferMemory(m_Device, buffer, bufferMemory, 0));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                              .commandPool = m_CommandBufferPool,
                                                              .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                              .commandBufferCount = 1 };
    VkCommandBuffer commandCopyBuffer;
    FFV_CHECK_VK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &commandCopyBuffer));

    VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                           .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };

    FFV_CHECK_VK_RESULT(vkBeginCommandBuffer(commandCopyBuffer, &beginInfo));
    VkBufferCopy copyRegion = { .srcOffset = 0, .dstOffset = 0, .size = size };
    vkCmdCopyBuffer(commandCopyBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    FFV_CHECK_VK_RESULT(vkEndCommandBuffer(commandCopyBuffer));

    VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &commandCopyBuffer };

    FFV_CHECK_VK_RESULT(vkQueueSubmit(m_Queue->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE));
    FFV_CHECK_VK_RESULT(vkQueueWaitIdle(m_Queue->GetQueue()));
}
} // namespace FFV
