#pragma once

#include "util/Types.h"

#include <GLFW/glfw3.h>
#include <string>

namespace FFV
{
class Window
{
public:
    Window(std::string title, U32 width, U32 height);
    ~Window();

    GLFWwindow* GetNativeWindow() { return m_Window; }

private:
    GLFWwindow* m_Window = nullptr;
};
} // namespace FFV
