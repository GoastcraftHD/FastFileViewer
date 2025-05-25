#pragma once

#include <GLFW/glfw3.h>

namespace FFV
{
class Window
{
public:
    Window();
    ~Window();

    GLFWwindow* GetNativeWindow()
    {
        return m_Window;
    }

private:
    GLFWwindow* m_Window = nullptr;
};
} // namespace FFV
