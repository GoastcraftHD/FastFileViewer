#pragma once

#include "renderer/PhysicalDevice.h"
#include "util/Assert.h"
#include "util/Types.h"

#include <fstream>
#include <string>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>
/*
 * In Debug mode a Assert is thrown when the result of the funtion is not VK_SUCCESS.
 *
 * In Release mode the code just gets executed and in case of a failure the program exits.
 * @param func: The vulkan function to check
 */
#define FFV_CHECK_VK_RESULT(func)                                                         \
    {                                                                                     \
        VkResult _result = func;                                                          \
        std::stringstream _ss;                                                            \
        _ss << "Error while processing vulkan code (" << string_VkResult(_result) << ")"; \
        FFV_ASSERT(_result == VK_SUCCESS, _ss.str(), exit(1));                            \
    }

#define FFV_DELETE_MOVE_COPY(type) \
    FFV_DELETE_MOVE(type);         \
    FFV_DELETE_COPY(type)
#define FFV_DELETE_MOVE(type) type(type&&) = delete
#define FFV_DELETE_COPY(type) type(const type&) = delete

namespace FFV
{
class Util
{
public:
    static std::vector<char> ReadBinaryFile(const std::string& path)
    {
        std::vector<char> buffer;
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        FFV_ASSERT(file.is_open(), "Failed to open file", return buffer);

        U64 fileSize = static_cast<U64>(file.tellg());
        buffer.resize(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        return buffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static U32 FindMemoryType(SharedPtr<PhysicalDevices> physicalDevice, U32 typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice->GetSelectedPhysicalDevice().PhysicalDevice, &memoryProperties);

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

    static void CreateBuffer(VkDevice device, SharedPtr<PhysicalDevices> physicalDevice, VkDeviceSize size,
                             VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer& buffer,
                             VkDeviceMemory& bufferMemory)
    {
        const VkBufferCreateInfo bufferCreateInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                      .size = size,
                                                      .usage = usageFlags,
                                                      .sharingMode = VK_SHARING_MODE_EXCLUSIVE };

        vkCreateBuffer(device, &bufferCreateInfo, VK_NULL_HANDLE, &buffer);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
        const VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, propertyFlags)
        };

        FFV_CHECK_VK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, VK_NULL_HANDLE, &bufferMemory));
        FFV_CHECK_VK_RESULT(vkBindBufferMemory(device, buffer, bufferMemory, 0));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static void CopyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandBufferPool, VkBuffer& srcBuffer,
                           VkBuffer& dstBuffer, VkDeviceSize size)
    {
        const VkCommandBufferAllocateInfo commandBufferAllocateInfo = { .sType =
                                                                            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                        .commandPool = commandBufferPool,
                                                                        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                        .commandBufferCount = 1 };
        VkCommandBuffer commandCopyBuffer;
        FFV_CHECK_VK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandCopyBuffer));

        const VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                     .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };

        FFV_CHECK_VK_RESULT(vkBeginCommandBuffer(commandCopyBuffer, &beginInfo));
        const VkBufferCopy copyRegion = { .srcOffset = 0, .dstOffset = 0, .size = size };
        vkCmdCopyBuffer(commandCopyBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        FFV_CHECK_VK_RESULT(vkEndCommandBuffer(commandCopyBuffer));

        const VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                          .commandBufferCount = 1,
                                          .pCommandBuffers = &commandCopyBuffer };

        FFV_CHECK_VK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        FFV_CHECK_VK_RESULT(vkQueueWaitIdle(queue));
    }
};
} // namespace FFV
