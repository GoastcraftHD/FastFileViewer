#pragma once

#include "util/Assert.h"

#include <expected>
#include <vulkan/vk_enum_string_helper.h>

/*
 * In Debug and DebugOpt mode a Asset is thrown when the result of the funtion is not VK_SUCCESS.
 * In Release mode the code just gets executed and in case of a failure the program exits.
 * @param func: The vulkan function to check
 */
#define FFV_CHECK_VK_RESULT(func)                                                      \
    {                                                                                  \
        VkResult result = func;                                                        \
        std::stringstream ss;                                                          \
        ss << "Error while processng vulkan code (" << string_VkResult(result) << ")"; \
        FFV_ASSERT(result == VK_SUCCESS, ss.str(), exit(1));                           \
    }

#define DELETE_MOVE_COPY(type) \
    DELETE_MOVE(type);         \
    DELETE_COPY(type)
#define DELETE_MOVE(type) type(type&&) = delete
#define DELETE_COPY(type) type(const type&) = delete
