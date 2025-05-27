#pragma once

#include "util/Assert.h"

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

#define DELETE_MOVE_COPY(type) \
    DELETE_MOVE(type);         \
    DELETE_COPY(type)
#define DELETE_MOVE(type) type(type&&) = delete
#define DELETE_COPY(type) type(const type&) = delete
