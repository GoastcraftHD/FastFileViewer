#pragma once

#include "util/Types.h"
#include "util/Util.h"

#include <GLFW/glfw3.h>
#include <string>

namespace FFV
{
class Window
{
public:
    Window(const std::string& title, U32 width, U32 height);
    ~Window();

    FFV_DELETE_MOVE_COPY(Window);

    U32 GetWidth();
    U32 GetHeight();
    GLFWwindow* GetNativeWindow() { return m_Window; }

private:
    GLFWwindow* m_Window = nullptr;
};
} // namespace FFV
