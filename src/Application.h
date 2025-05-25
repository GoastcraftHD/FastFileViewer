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

private:
    SharedPtr<Window> m_Window;
    SharedPtr<Renderer> m_Renderer;
};
} // namespace FFV
