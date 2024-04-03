#include "Window.h"

#include <iostream>

namespace FFV
{
    Window::Window(int width, int height, std::string name)
        : m_Width(width), m_Height(height), m_Name(name)
    {
        Init();
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::Init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
        {
            std::cerr << "Failed to create window surface" << std::endl;
        }
    }
}
