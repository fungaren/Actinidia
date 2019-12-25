#include "pch.h"
#include "../Tools/Common/Window.h"
#include "../Tools/Common/ResourcePack.h"

std::shared_ptr<ImageMatrix> img;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ResourcePack pack = ResourcePack::parsePack("demo.res");
    char* p;
    uint32_t size;
    static Window w;
    if (pack.readResource(L"./demo/actinidia.png", &p, &size))
    {
        img = ImageMatrixFactory::fromPngBuffer(p, size);
        w.setPaintHandler([](const GdiCanvas& gdi) {
            auto w_size = w.getSize();
            auto temp = ImageMatrixFactory::createBufferImage(
                w_size.first, w_size.second, Canvas::Constant::white
            );
            PiCanvas::blend(temp, img, 0, 0, 255);
            gdi.paste(temp, 0, 0);
        });
        w.create(L"demo");
    }
    
    return 0;
}
