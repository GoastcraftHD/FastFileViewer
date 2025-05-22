#include "FastFileViewerPCH.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
    FFV::Log::Init();
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    FFV_LOG("{0} extensions supported!", extensionCount);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
