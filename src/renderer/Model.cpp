#include "FastFileViewerPCH.h"

#include "Model.h"

namespace FFV
{
Model::Model(const std::vector<Vertex>& vertices, const std::vector<U32>& indices, VkDevice device,
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

void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
    const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Util::CreateBuffer(m_Device, m_PhysicalDevices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                       stagingBufferMemory);

    void* dataStaging;
    FFV_CHECK_VK_RESULT(vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &dataStaging));

    memcpy(dataStaging, vertices.data(), bufferSize);
    vkUnmapMemory(m_Device, stagingBufferMemory);

    Util::CreateBuffer(m_Device, m_PhysicalDevices, bufferSize,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

    Util::CopyBuffer(m_Device, m_Queue->GetQueue(), m_CommandBufferPool, stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, stagingBufferMemory, VK_NULL_HANDLE);

    FFV_TRACE("Created vertex buffer with {0} vertices!", vertices.size());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model::CreateIndexBuffer(const std::vector<U32>& indices)
{
    const VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Util::CreateBuffer(m_Device, m_PhysicalDevices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                       stagingBufferMemory);

    void* dataStaging;
    FFV_CHECK_VK_RESULT(vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &dataStaging));

    memcpy(dataStaging, indices.data(), bufferSize);
    vkUnmapMemory(m_Device, stagingBufferMemory);

    Util::CreateBuffer(m_Device, m_PhysicalDevices, bufferSize,
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

    Util::CopyBuffer(m_Device, m_Queue->GetQueue(), m_CommandBufferPool, stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(m_Device, stagingBufferMemory, VK_NULL_HANDLE);

    FFV_TRACE("Created index buffer with {0} indicies!", indices.size());
}
} // namespace FFV
