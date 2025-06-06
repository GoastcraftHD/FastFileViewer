#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace FFV
{
class Shader
{
public:
    /*
     * @param path: path based of assets/shaders e.g.: default.vert.spv
     */
    Shader(VkDevice device, std::string path);
    ~Shader();

    VkShaderStageFlagBits GetShaderStage() { return m_ShaderStage; }
    std::string GetShaderStageName() { return m_ShaderStageName; }

private:
    /*
     * @param filename: name of the file with extension
     */
    void GetShaderStageFromName(std::string filename);

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkShaderModule m_Module = VK_NULL_HANDLE;
    VkShaderStageFlagBits m_ShaderStage;
    std::string m_ShaderStageName;
};
} // namespace FFV
