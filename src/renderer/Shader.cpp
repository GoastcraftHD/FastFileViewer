#include "FastFileViewerPCH.h"

#include "Shader.h"

#include "util/Util.h"

#include <string>
#include <vector>

namespace FFV
{
Shader::Shader(VkDevice device, std::string path)
{
    std::string relativPath = "";

#if defined(FFV_DEBUG)
    relativPath = "../../../assets/shaders/";
#elif defined(FFV_RELEASE)
    relativPath = "./assets/shaders/";
#endif

    relativPath += path;

    std::vector<char> buffer = ReadBinaryFile(relativPath);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                                        .codeSize = static_cast<U32>(buffer.size()),
                                                        .pCode = reinterpret_cast<const U32*>(buffer.data()) };

    FFV_CHECK_VK_RESULT(vkCreateShaderModule(m_Device, &shaderModuleCreateInfo, VK_NULL_HANDLE, &m_Module));

    GetShaderStageFromName(path);
}

Shader::~Shader() { vkDestroyShaderModule(m_Device, m_Module, VK_NULL_HANDLE); }

void Shader::GetShaderStageFromName(std::string filename)
{
    if (filename.ends_with("vert.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_VERTEX_BIT;
        m_ShaderStageName = "vertex";
    }
    else if (filename.ends_with("frag.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
        m_ShaderStageName = "fragment";
    }
    else if (filename.ends_with("rgen.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        m_ShaderStageName = "raygeneration";
    }
    else if (filename.ends_with("rmiss.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_MISS_BIT_KHR;
        m_ShaderStageName = "miss";
    }
    else if (filename.ends_with("rchit.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        m_ShaderStageName = "closesthit";
    }
    else if (filename.ends_with("rcall.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_CALLABLE_BIT_KHR;
        m_ShaderStageName = "callable";
    }
    else if (filename.ends_with("rint.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        m_ShaderStageName = "intersection";
    }
    else if (filename.ends_with("rahit.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        m_ShaderStageName = "anyhit";
    }
    else if (filename.ends_with("comp.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
        m_ShaderStageName = "compute";
    }
    else if (filename.ends_with("mesh.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_MESH_BIT_EXT;
        m_ShaderStageName = "mesh";
    }
    else if (filename.ends_with("task.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_TASK_BIT_EXT;
        m_ShaderStageName = "amplification";
    }
    else if (filename.ends_with("geom.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
        m_ShaderStageName = "geometry";
    }
    else if (filename.ends_with("tesc.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        m_ShaderStageName = "hull";
    }
    else if (filename.ends_with("tese.spv"))
    {
        m_ShaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        m_ShaderStageName = "domain";
    }

    FFV_ASSERT(false, "Invalid file name", return);
}
} // namespace FFV
