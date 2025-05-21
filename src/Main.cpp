#include "FastFileViewerPCH.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

    u_int32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::printf("%i extensions supported!\n", extensionCount);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
