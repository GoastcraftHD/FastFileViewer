#include "FastFileViewerPCH.h"

#include "Application.h"

#if defined(FFV_WINDOWS) && defined(FFV_RELEASE)
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    FFV::Application app{};
    app.Run();
}
#else
int main()
{
    FFV::Application app{};
    app.Run();
}
#endif
