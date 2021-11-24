/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
    // #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef max

    #if defined _M_IX86
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'"\
    " version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #elif defined _M_IA64
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'"\
    " version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #elif defined _M_X64
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'"\
    " version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #else
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'"\
    " version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #endif
#elif defined _GTK
    #include <gtk/gtk.h>
#else
#error unsupported platform
#endif
#include <map>
#include <filesystem>

#include "ResourcePack.h"
#include "Timer.h"

#include "resource.h"
#include "actinidia.h"

// In Direct mode, the name of resource folder must be "game"
bool bDirectMode;
pResourcePack pack;
const string_t USER_DATA_PATH = ustr("game.data");
// Use UTF-8 encoding
std::map<const std::string, std::string> user_data;
// save reference to ImageMatrix handles
std::map<size_t, pImageMatrix> im_handles;
// GUI
Window w;
size_t window_width = 1024;
size_t window_height = 768;
Timer* timer;

#ifdef _WIN32
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    // Resource file or resource folder
    string_t game_res = ustr("game.res");
    if (*lpCmdLine == '\0')
#elif defined _GTK
int main(int argc, char** argv)
{
    char *lpCmdLine = argv[1];
    // Resource file or resource folder
    string_t game_res = ustr("game.res");
    if (argc == 1)
#else
#error unsupported platform
#endif
    {
        // No resource file specified, use default
        try {
            pack = ResourcePack::parsePack(game_res);
            bDirectMode = false;
        }
        catch (ustr_error& e) {
            // No default resource file found, try direct mode
            if (std::filesystem::exists("game") && std::filesystem::is_directory("game"))
                bDirectMode = true;
            else {
                w.alert(ustr("Failed to load default resource file \"game.res\": ") + e.what(), ustr("Error"), Window::ERROR);
                return 1;
            }
        }
    }
    else
    {
        std::filesystem::path path = lpCmdLine;
        if (!std::filesystem::exists(path)) {
            return 1;
        }
        // Use specified resource file
        try {
            pack = ResourcePack::parsePack(path.to_ustr());
            bDirectMode = false;
            game_res = path.filename().to_ustr();
        }
        catch (ustr_error& e) {
            w.alert(ustr("Failed to load the resource file ") + e.what(), ustr("Error"), Window::ERROR);
            return 1;
        }
    }

    // Load user data
    std::ifstream in(USER_DATA_PATH);
    char buff[1024];
    if (in.good()) {
        while (in.getline(buff, 1024)) {
            if (buff[0] == '#') continue;           // # this is comment
            const char* pos = strchr(buff, '=');    // eg. best = 999
            if (pos != NULL) {
                std::string key(buff, pos - buff);
                user_data[key] = std::string(pos + 1);
            }
        }
    }
    in.close();

    // set callbackss
    w.setLButtonDownHandler(OnLButtonDown);
    w.setLButtonUpHandler(OnLButtonUp);
    w.setPaintHandler(OnPaint);
    w.setKeyDownHandler(OnKeyDown);
    w.setKeyUpHandler(OnKeyUp);
    w.setMouseMoveHandler(OnMouseMove);
    w.setMouseWheelHandler(OnMouseWheel);
    w.setGetFocusHandler(OnSetFocus);
    w.setLoseFocusHandler(OnKillFocus);
    w.setExitCallback([]() {
        timer->end();
        OnClose();

        // save user data
        std::ofstream out(USER_DATA_PATH);
        for (auto& p : user_data) {
            out << p.first.c_str() << '=' << p.second.c_str() << '\n';
        }
        out.close();

        OnClean();
        return true;
    });

    // Initialize Lua environment and run script
    if (LuaInit())
    {
#ifdef _WIN32
        RECT rc = { 0, 0, (LONG)window_width, (LONG)window_height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        // To create a window with specified size of client area.
        w.create(
            game_res, // title
            (uint16_t)(rc.right - rc.left), // width
            (uint16_t)(rc.bottom - rc.top), // height
            MIN_WIDTH, // min width
            MIN_HEIGHT, // min height
            NULL,
            LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP))
        );
#elif defined _GTK
        w.create(
            game_res,
            window_width,
            window_height,
            MIN_WIDTH, // min width
            MIN_HEIGHT  // min height
        );
#else
#error unsupported platform
#endif
        timer = new Timer();
        timer->begin(17ms, [] {
            w.refresh();
        }, 1000ms);
    }
    return 0;
}
