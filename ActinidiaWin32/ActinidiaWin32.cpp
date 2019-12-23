#include "pch.h"
#include "../Tools/Common/Window.h"
#include "../Tools/Common/ResourcePack.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    Window w;
    w.create(L"demo");

    ResourcePack pack = ResourcePack::parsePack("demo.res");
    char* p;
    uint32_t size;
    if (pack.readResource(L"./demo/actinidia.png", &p, &size))
    {
        ImageMatrix img = ImageMatrixFactory::fromPngBuffer(p, size);
        
    }
}
