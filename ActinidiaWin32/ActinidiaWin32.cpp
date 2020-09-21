#ifdef _WIN32
    #include "pch.h"
#endif /* _WIN32 */
#ifdef _GTK
    
#endif
#include <iostream>
#include <string>
#include <sstream>
#include <future>
#include <chrono>
#include <fstream>
#include <experimental/filesystem>
#include <thread>
#include <map>

#define MIN_WIDTH 200
#define MIN_HEIGHT 100

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
std::wstring user_data_path;
std::map<const std::string, std::string> user_data;
// GUI
Window w;
size_t window_width = 1024;
size_t window_height = 768;
Timer* timer;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    // resource file or resource folder
    std::wstring game_res = L"game.res";
    if (*lpCmdLine == '\0')
    {
        try {
            // no resource file specified, use default
            pack = ResourcePack::parsePack(game_res);
            bDirectMode = false;
        }
        catch (std::runtime_error e) {
            // no default resource file found, try direct mode
            if (std::filesystem::exists("game") && std::filesystem::is_directory("game"))
                bDirectMode = true;
            else {
                MessageBox(NULL, L"Failed to load default resource file \"game.res\"", L"Error", MB_ICONERROR);
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
            catch (std::runtime_error e) {
                MessageBox(NULL, L"Failed to load the resource file", L"Error", MB_ICONERROR);
                return 1;
            }
        }
        else {
            MessageBox(NULL, L"Invalid resource file", L"Error", MB_ICONERROR);
            return 1;
        }
    }

    // load user data
    if (bDirectMode) {
        user_data_path = L"game.data";
    }
    else {
        user_data_path = game_res + L".data";
    }
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

    // set callbacks
    
    w.setLButtonDownHandler(OnLButtonDown);
    w.setLButtonUpHandler(OnLButtonUp);
    w.setPaintHandler(OnPaint);
    w.setKeyDownHandler(OnKeyDown);
    w.setKeyUpHandler(OnKeyUp);
    w.setMouseMoveHandler(OnMouseMove);
    w.setMouseWheelHandler(OnMouseWheel);
    w.setElseHandler([](uint32_t message, WPARAM, LPARAM lParam) {
        switch (message) {
        case WM_GETMINMAXINFO:
            MINMAXINFO* lpMMI;  // Minimum size
            lpMMI = (MINMAXINFO*)lParam;
            lpMMI->ptMinTrackSize.x = MIN_WIDTH;
            lpMMI->ptMinTrackSize.y = MIN_HEIGHT;
            return true;
        case WM_SETFOCUS:
            OnSetFocus();
            return true;
        case WM_KILLFOCUS:
            OnKillFocus();
            return true;
        }
        return false;
    });
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
        RECT rc = { 0, 0, (LONG)window_width, (LONG)window_height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        // To create a window with specified size of client area.
        w.create(game_res, (uint16_t)(rc.right - rc.left), (uint16_t)(rc.bottom - rc.top), NULL,
            LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP)));
        timer = new Timer();
        timer->begin(std::chrono::milliseconds(17), [] {
            w.refresh();
        });
    }
    return 0;
}
