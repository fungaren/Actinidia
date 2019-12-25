#include "pch.h"
#include "../Tools/Common/Window.h"
#include "../Tools/Common/ResourcePack.h"

// callbacks
bool OnInit();
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

bool bDirectMode;
// resource file or resource folder
std::wstring game_res = L"game.res";
ResourcePack pack;
// user data
std::wstring user_data_path;
std::map<const std::string, std::string> user_data;
// GUI
Window w;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    int argc;
    LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);
    if (argc == 1)
    {
        try {
            // no resource file specified, use default
            pack = std::move(ResourcePack::parsePack(game_res));
            bDirectMode = false;
        }
        catch (std::runtime_error e) {
            MessageBox(NULL, L"Failed to load default resource file \"game.res\"", L"Error", MB_OK);
            LocalFree(argv);
            return 1;
        }
    }
    else if (argc == 2)
    {
        std::filesystem::path path = argv[1];
        if (!std::filesystem::exists(path)) {
            LocalFree(argv);
            return 1;
        }
        // Directory: Direct Mode
        if (std::filesystem::is_directory(path))
        {
            bDirectMode = true;
            game_res = argv[1];
        }
        // Resource file
        else if (path.extension() == L"res")
        {
            try {
                // use specified resource file
                pack = std::move(ResourcePack::parsePack(path));
                bDirectMode = false;
                game_res = path.filename();
            }
            catch (std::runtime_error e) {
                MessageBox(NULL, L"Failed to load the resource file", L"Error", MB_OK);
                LocalFree(argv);
                return 1;
            }
        }
        else {
            MessageBox(NULL, L"Invalid resource file", L"Error", MB_OK);
            LocalFree(argv);
            return 1;
        }
    }
    LocalFree(argv);

    // load user data
    if (bDirectMode) {
        user_data_path = std::filesystem::path(game_res).filename().wstring() + L".data";
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
    w.setElseHandler([](uint32_t message, WPARAM, LPARAM) {
        switch (message) {
        case WM_SETFOCUS:
            OnSetFocus();
            return true;
        case WM_KILLFOCUS:
            OnKillFocus();
            return true;
        }
        return false;
    });
    w.setExitCallback([](uint32_t, WPARAM, LPARAM) {
        OnClose();

        // save user data
        std::ofstream out(user_data_path);
        for (auto& p : user_data) {
            out << p.first.c_str() << '=' << p.second.c_str() << '\n';
        }
        out.close();

        OnClean();
    });
    w.create(L"demo");    
    return 0;
}
