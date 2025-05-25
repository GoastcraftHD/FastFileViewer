#include "Window.h"

#include "FastFileViewerPCH.h"

namespace FFV
{
Window::Window()
{
    m_Window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);
    FFV_ASSERT(m_Window, "Couldn't create window!", ;);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
}
} // namespace FFV
