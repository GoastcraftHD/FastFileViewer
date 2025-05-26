#include "FastFileViewerPCH.h"

#include "Application.h"

#include "util/Log.h"
#include "util/Types.h"
#include "util/Util.h"

#include <GLFW/glfw3.h>

namespace FFV
{

Application* Application::s_Instance = nullptr;

Application::Application()
{
    FFV_ASSERT(!s_Instance, "Application already exists!", return);
    s_Instance = this;

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
    // glfwTerminate();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_Window->GetNativeWindow()))
    {
        glfwPollEvents();
    }
}
} // namespace FFV
