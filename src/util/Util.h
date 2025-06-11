#pragma once

#include "util/Assert.h"
#include "util/Types.h"

#include <fstream>
#include <string>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>

/*
 * In Debug mode a Assert is thrown when the result of the funtion is not VK_SUCCESS.
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
    static std::vector<char> ReadBinaryFile(std::string path)
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
};
} // namespace FFV
