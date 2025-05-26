#pragma once

#include "Window.h"
#include "renderer/Renderer.h"
#include "util/Types.h"

namespace FFV
{
class Application
{
public:
    Application();
    ~Application();

    void Run();

    SharedPtr<Window>& GetWindow()
    {
        return m_Window;
    }

    static Application& Get()
    {
        return *s_Instance;
    }

private:
    static Application* s_Instance;
    SharedPtr<Window> m_Window;
    SharedPtr<Renderer> m_Renderer;
};
} // namespace FFV
