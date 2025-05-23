#include "FastFileViewerPCH.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
    FFV::Log::Init();

    bool success = glfwInit();
    FFV_ASSERT(success, "Couldn't initilize GLFW!", return 1)

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);
    FFV_ASSERT(window, "Couldn't create window!", return 1)

    U32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    FFV_LOG("{0} extensions supported!", extensionCount);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
