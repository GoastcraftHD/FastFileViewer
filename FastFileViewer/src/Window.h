#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <string>

namespace FFV
{
    class Window
    {
    public:
        Window(int width, int height, std::string name);
        ~Window();

        bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
        VkExtent2D GetExtent() { return { (uint32_t)m_Width, (uint32_t)m_Height }; }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    private:
        void Init();

        const int m_Width;
        const int m_Height;
        std::string m_Name;

        GLFWwindow* m_Window;
    };
}
