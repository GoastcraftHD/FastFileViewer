#pragma once

#include <vulkan/vulkan.h>

namespace FFV
{
class Renderer
{
public:
    Renderer();
    ~Renderer();

private:
    void CreateInstance();

private:
    VkInstance m_Instance = nullptr;
};
} // namespace FFV
