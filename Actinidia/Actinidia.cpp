/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
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
#endif /* _WIN32 */
#ifdef _GTK
    #include <gtk/gtk.h>
#endif
#include <string>
#include <sstream>
#include <filesystem>
#include <map>

#include "../Tools/Common/Window.h"
#include "../Tools/Common/ResourcePack.h"
#include "../Tools/Common/Timer.h"
#include "resource.h"

// callbacks
bool LuaInit();
void OnClean();
void OnSetFocus();
void OnKillFocus();
void OnPaint(const GdiCanvas& gdi);
void OnClose();
void OnKeyDown(int key);
void OnKeyUp(int key);
void OnLButtonDown(uint32_t, int x, int y);
void OnLButtonUp(uint32_t, int x, int y);
void OnMouseMove(uint32_t, int x, int y);
void OnMouseWheel(uint32_t, short zDelta, int x, int y);

// In Direct mode, the name of resource folder must be "game"
bool bDirectMode;
pResourcePack pack;
// user data
#ifdef _WIN32
std::wstring user_data_path;
#endif /* _WIN32 */
#ifdef _GTK
std::string user_data_path;
#endif /* _GTK */
std::map<const std::string, std::string> user_data;
// save reference to ImageMatrix handles
std::map<size_t, pImageMatrix> im_handles;
// GUI
Window w;
size_t window_width = 1024;
size_t window_height = 768;
const int MIN_WIDTH = 200;
const int MIN_HEIGHT = 100;
Timer* timer;

#ifdef _WIN32
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    // resource file or resource folder
    std::wstring game_res = "game.res";
    if (*lpCmdLine == '\0')
#endif /* _WIN32 */
#ifdef _GTK
int main(int argc, char** argv)
{
    char *lpCmdLine = argv[1];
    // resource file or resource folder
    std::string game_res = "game.res";
    if (argc == 1)
#endif /* _GTK */
    {
        // no resource file specified, use default
        try {
            pack = ResourcePack::parsePack(game_res);
            bDirectMode = false;
        }
        catch (std::runtime_error& e) {
            // no default resource file found, try direct mode
            if (std::filesystem::exists("game") && std::filesystem::is_directory("game"))
                bDirectMode = true;
            else {
                w.alert("Failed to load default resource file \"game.res\"", "Error", Window::ERROR);
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
        // use specified resource file
        if (path.extension() == L".res")
        {
            try {
                pack = ResourcePack::parsePack(path);
                bDirectMode = false;
                game_res = path.filename();
            }
            catch (std::runtime_error& e) {
                w.alert("Failed to load the resource file", "Error", Window::ERROR);
                return 1;
            }
        }
        else {
            w.alert("Invalid resource file", "Error", Window::ERROR);
            return 1;
        }
    }

#ifdef _WIN32
    // load user data
    if (bDirectMode)
        user_data_path = L"game.data";
    else
        user_data_path = L".data";
#endif /* _WIN32 */
#ifdef _GTK
    // load user data
    if (bDirectMode)
        user_data_path = "game.data";
    else
        user_data_path = ".data";
#endif /* _GTK */

    std::ifstream in(user_data_path);
    char buff[1024];
    if (in.good()) {
        while (in.getline(buff, 1024)) {
            if (buff[0] == '#') continue;			// # this is comment
            const char* pos = strchr(buff, '=');	// eg. best = 999
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
        std::ofstream out(user_data_path);
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
#endif /* _WIN32 */
#ifdef _GTK
        w.create(
            game_res,
            window_width,
            window_height,
            MIN_WIDTH, // min width
            MIN_HEIGHT  // min height
        );
#endif /* _GTK */
        timer = new Timer();
        timer->begin(17ms, [] {
            w.refresh();
        }, 1000ms);
    }
    return 0;
}
