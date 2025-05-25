#include "Application.h"

#include "util/Types.h"

#define GLFW_INCLUDE_VULKAN
#include "FastFileViewerPCH.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>

namespace FFV
{
Application::Application()
{
    Log::Init();

    FFV_ASSERT(glfwInit(), "Couldn't initilize GLFW!", exit(1));

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = MakeShared<Window>();

    U32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    FFV_LOG("{0} extensions supported!", extensionCount);

    m_Renderer = MakeShared<Renderer>();
}

Application::~Application()
{
    glfwTerminate();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_Window->GetNativeWindow()))
    {
        glfwPollEvents();
    }
}
} // namespace FFV
