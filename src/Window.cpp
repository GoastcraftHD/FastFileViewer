#include "FastFileViewerPCH.h"

#include "Window.h"

#include "GLFW/glfw3.h"

namespace FFV
{
Window::Window(const std::string& title, U32 width, U32 height)
{
    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    FFV_ASSERT(m_Window, "Couldn't create window!", ;);

    FFV_TRACE("Created window '{0}' ({1}x{2})", title, width, height);
}

Window::~Window() { glfwDestroyWindow(m_Window); }

U32 Window::GetWidth() const
{
    I32 width;
    glfwGetWindowSize(m_Window, &width, nullptr);
    return static_cast<U32>(width);
}

U32 Window::GetHeight() const
{
    I32 height;
    glfwGetWindowSize(m_Window, nullptr, &height);
    return static_cast<U32>(height);
}
} // namespace FFV
