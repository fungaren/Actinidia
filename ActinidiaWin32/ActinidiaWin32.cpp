#include "pch.h"
#include "../Tools/Common/Window.h"
#include "../Tools/Common/ResourcePack.h"

#define MIN_WIDTH 200
#define MIN_HEIGHT 100
int WIN_WIDTH = 1024;
int WIN_HEIGHT = 768;

bool InitApp(LPCTSTR lpCmdLine)
{
    if (0 == lstrcmp(lpCmdLine, L"DirectMode"))
    {
        bDirectMode = true;
    }
    else if (!InitLoader())
    {
        MessageBoxA(NULL, "Fail to load resources!", STR_ERR_TITLE, MB_ICONERROR);
        return false;
    }

    // load user data
    std::ifstream in(bDirectMode ? "res\\data" : "data", std::ios::in);
    char buff[1024];
    if (in.good()) {
        while (in.getline(buff, 1024)) {
            if (buff[0] == '#') continue;			// # this is comment
            const char* pos = strchr(buff, '=');	// eg. best = 999
            if (pos != NULL) {
                std::string key(buff, pos - buff);
                prop[key] = std::string(pos + 1);
            }
        }
    }
    in.close();

    BASS_Init(-1, 44100, 0, 0, 0);

    L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "CreateImage", CreateImage);
    lua_register(L, "CreateImageEx", CreateImageEx);
    lua_register(L, "CreateTransImage", CreateTransImage);
    lua_register(L, "DeleteImage", DeleteImage);
    lua_register(L, "GetWidth", GetWidth);
    lua_register(L, "GetHeight", GetHeight);
    lua_register(L, "GetText", GetText);
    lua_register(L, "GetImage", GetImage);
    lua_register(L, "GetSound", GetSound);
    lua_register(L, "EmptyStack", EmptyStack);
    lua_register(L, "PasteToImage", PasteToImage);
    lua_register(L, "PasteToImageEx", PasteToImageEx);
    lua_register(L, "AlphaBlend", AlphaBlend);
    lua_register(L, "AlphaBlendEx", AlphaBlendEx);
    lua_register(L, "PasteToWnd", PasteToWnd);
    lua_register(L, "PasteToWndEx", PasteToWndEx);
    lua_register(L, "StopSound", StopSound);
    lua_register(L, "SetVolume", SetVolume);
    lua_register(L, "PlaySound", PlaySound);
    lua_register(L, "Screenshot", Screenshot);
    lua_register(L, "GetSetting", GetSetting);
    lua_register(L, "SaveSetting", SaveSetting);

    LPBYTE pMem;
    long size = GetFile("res\\config.ini", &pMem);	// Load configure
    if (size > 0)
    {
        std::string conf_str((char*)pMem, size);
        conf_str += '\n';

        // Get client size
        int result = conf_str.find("preferred_width", 0);
        if (result >= 0) {
            int w_begin = 1 + conf_str.find('=', result);
            int w_end = conf_str.find('\n', w_begin);
            result = std::stoi(conf_str.substr(w_begin, w_end - w_begin));
            if (result > MIN_WIDTH) WIN_WIDTH = result;
        }
        result = conf_str.find("preferred_height", 0);
        if (result >= 0) {
            int w_begin = 1 + conf_str.find('=', result);
            int w_end = conf_str.find('\n', w_begin);
            result = std::stoi(conf_str.substr(w_begin, w_end - w_begin));
            if (result > MIN_HEIGHT) WIN_HEIGHT = result;
        }
    }

    size = GetFile("res\\lua\\main.lua", &pMem);			// Load scripts & set screen size
    if (size > 0 && luaL_loadbuffer(L, (char*)pMem, size, "line") == 0 && lua_pcall(L, 0, LUA_MULTRET, 0) == 0)
    {
        lua_getglobal(L, "core");
        lua_pushinteger(L, WIN_WIDTH);
        lua_setfield(L, -2, "screenwidth");
        lua_getglobal(L, "core");
        lua_pushinteger(L, WIN_HEIGHT);
        lua_setfield(L, -2, "screenheight");
        return true;
    }
    else
    {
        MessageBoxA(NULL, "Fail to load main script!", STR_ERR_TITLE, MB_ICONERROR);
        return false;
    }
}

inline void CleanAll()
{
    if (!bDirectMode)
        fclose(pPackFile);

    BASS_Free();
    lua_close(L);

    // save user data
    std::ofstream out(bDirectMode ? "res\\data" : "data", std::ios::out);
    for (std::pair<const std::string, std::string> p in prop) {
        out << p.first.c_str() << '=' << p.second.c_str() << '\n';
    }
    out.close();
}

pImageMatrix img;
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
