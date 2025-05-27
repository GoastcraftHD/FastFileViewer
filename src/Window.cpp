#include "FastFileViewerPCH.h"

#include "Window.h"

namespace FFV
{
Window::Window(std::string title, U32 width, U32 height)
{
    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    FFV_ASSERT(m_Window, "Couldn't create window!", ;);

    FFV_TRACE("Created window '{0}' ({1}x{2})", title, width, height);
}

Window::~Window() { glfwDestroyWindow(m_Window); }
} // namespace FFV
