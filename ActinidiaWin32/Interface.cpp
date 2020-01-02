#include "pch.h"
#undef PlaySound 
#include "../Tools/Common/Window.h"
#include "../Tools/Common/Canvas.h"
#include "../Tools/Common/ImageMatrix.h"
#include "../Tools/Common/ResourcePack.h"
#include "lua/lua.hpp"

#pragma comment(lib, "lua/lua.lib")
#include "bass/bass.h"
#pragma comment(lib, "bass/bass.lib")

lua_State* L = nullptr;
extern pResourcePack pack;
extern Window w;
extern bool bDirectMode;
extern std::map<const std::string, std::string> user_data;

//==============================================
//=        Global functions for lua            =
//==============================================

// lua: Image CreateImage(width, height), Create Image with Black bg, must DeleteImage() in OnClose()
int CreateImage(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    int width = (int)lua_tointeger(L, 1);
    int height = (int)lua_tointeger(L, 2);
    lua_pop(L, 2);
    auto g = ImageMatrixFactory::createBufferImage(width, height);
    lua_pushinteger(L, (lua_Integer)g);
    return 1;
}

// lua: Image CreateImageEx(width, height, color), Create Image with specific bg, must DeleteImage() in OnClose()
int CreateImageEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 3) return 0;
    int width = (int)lua_tointeger(L, 1);
    int height = (int)lua_tointeger(L, 2);
    auto c = lua_tointeger(L, 3);
    lua_pop(L, 3);
    auto g = ImageMatrixFactory::createBufferImage(width, height, Canvas::color(c));
    lua_pushinteger(L, (lua_Integer)g);
    return 1;
}

// lua: Image CreateTransImage(width, height), Create Transparent Image, must DeleteImage() in OnClose()
int CreateTransImage(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    int width = (int)lua_tointeger(L, 1);
    int height = (int)lua_tointeger(L, 2);
    lua_pop(L, 2);
    auto g = ImageMatrixFactory::createBufferImage(width, height, Canvas::Constant::alpha);
    lua_pushinteger(L, (lua_Integer)g);
    return 1;
}

// lua: void DeleteImage(g)
int DeleteImage(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    auto g = (ImageMatrix*)lua_tointeger(L, 1);
    lua_pop(L, 1);
    delete g;
    return 0;
}

// lua: int GetWidth(g)
int GetWidth(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    auto g = (ImageMatrix*)lua_tointeger(L, 1);
    lua_pop(L, 1);
    lua_pushinteger(L, g->getWidth());
    return 1;
}

// lua: int GetHeight(g)
int GetHeight(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    auto g = (ImageMatrix*)lua_tointeger(L, 1);
    lua_pop(L, 1);
    lua_pushinteger(L, g->getHeight());
    return 1;
}

// lua: string GetText(pathname)
int GetText(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    std::string f = lua_tostring(L, 1);
    lua_pop(L, 1);
    char* p;
    uint32_t size;
    if (bDirectMode) {
        std::ifstream in("./game/" + f, std::ios::binary);
        if (in.good()) {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            p = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(p, size);

            lua_pushlstring(L, p, size);
            return 1;
        }
    }
    else
        if (pack->readResource("./game/" + f, &p, &size))
        {
            lua_pushlstring(L, p, size);
            return 1;
        }
    lua_pushnil(L);
    return 1;
}

// lua: image GetImage(pathname), must DeleteImage() in OnClose()
int GetImage(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    std::string f = lua_tostring(L, 1);
    lua_pop(L, 1);
    char* p;
    uint32_t size;
    pImageMatrix g;
    if (bDirectMode) {
        try {
            if (f.substr(f.size() - 3, 3) == "png")
                g = ImageMatrixFactory::fromPngFile(("./game/" + f).c_str());

            else
                g = ImageMatrixFactory::fromJpegFile(("./game/" + f).c_str());
            lua_pushinteger(L, (lua_Integer)g);
            return 1;
        }
        catch (std::runtime_error e) {
            w.alert(e.what(), "ERROR", MB_ICONERROR);
        }
    }
    else
        if (pack->readResource("./game/" + f, &p, &size))
        {
            try {
                if (f.substr(f.size() - 3, 3) == "png")
                    g = ImageMatrixFactory::fromPngBuffer(p, size);
                else 
                    g = ImageMatrixFactory::fromJpegBuffer(p, size);
                lua_pushinteger(L, (lua_Integer)g);
                return 1;
            }
            catch (std::runtime_error e) {
                w.alert(e.what(), "ERROR", MB_ICONERROR);
            }
        }
    lua_pushnil(L);
    return 1;
}

// lua: sound GetSound(pathname, b_loop), if b_loop set true, must StopSound() in OnClose()
int GetSound(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    std::string f = lua_tostring(L, 1);
    int b_loop = lua_toboolean(L, 2);
    lua_pop(L, 2);
    char* p;
    uint32_t size;
    if (bDirectMode) {
        std::ifstream in("./game/" + f, std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            p = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(p, size);

            HSTREAM sound = BASS_StreamCreateFile(TRUE, p, 0, size,
                b_loop ? BASS_SAMPLE_LOOP : 0);
            if (sound)
            {
                lua_pushinteger(L, sound);
                return 1;
            }
        }
        in.close();
    }
    else 
        if (pack->readResource("./game/" + f, &p, &size))
        {
            HSTREAM sound = BASS_StreamCreateFile(TRUE, p, 0, size,
                b_loop ? BASS_SAMPLE_LOOP : 0);
            if (sound)
            {
                lua_pushinteger(L, sound);
                return 1;
            }
        }
    lua_pushnil(L);
    return 1;
}

// lua: void PasteToImage(gDest, gSrc, xDest, yDest)
int PasteToImage(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 4) return 0;
    auto gDest = (ImageMatrix*)lua_tointeger(L, 1);
    auto gSrc = (ImageMatrix*)lua_tointeger(L, 2);
    int x = (int)lua_tointeger(L, 3);
    int y = (int)lua_tointeger(L, 4);
    lua_pop(L, 4);
    PiCanvas::blend(gDest, gSrc, x, y, 255);
    return 0;
}

// lua: void PasteToImageEx(gDest, gSrc, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight)
int PasteToImageEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 10) return 0;
    auto gDest = (ImageMatrix*)lua_tointeger(L, 1);
    auto gSrc = (ImageMatrix*)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    int DestWidth = (int)lua_tointeger(L, 5);
    int DestHeight = (int)lua_tointeger(L, 6);
    int xSrc = (int)lua_tointeger(L, 7);
    int ySrc = (int)lua_tointeger(L, 8);
    int SrcWidth = (int)lua_tointeger(L, 9);
    int SrcHeight = (int)lua_tointeger(L, 10);
    lua_pop(L, 10);
    PiCanvas::blend(gDest, gSrc, xDest, yDest, DestWidth, DestHeight,
        xSrc, ySrc, SrcWidth, SrcHeight, 255);
    return 0;
}

// lua: void AlphaBlend(gDest, gSrc, xDest, yDest, Opacity)
int AlphaBlend(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 5) return 0;
    auto gDest = (ImageMatrix*)lua_tointeger(L, 1);
    auto gSrc = (ImageMatrix*)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    unsigned char Opacity = (unsigned char)lua_tointeger(L, 5);
    lua_pop(L, 5);
    PiCanvas::blend(gDest, gSrc, xDest, yDest, Opacity);
    return 0;
}

// lua: void AlphaBlendEx(gDest, gSrc, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight, Opacity)
int AlphaBlendEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 11) return 0;
    auto gDest = (ImageMatrix*)lua_tointeger(L, 1);
    auto gSrc = (ImageMatrix*)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    int DestWidth = (int)lua_tointeger(L, 5);
    int DestHeight = (int)lua_tointeger(L, 6);
    int xSrc = (int)lua_tointeger(L, 7);
    int ySrc = (int)lua_tointeger(L, 8);
    int SrcWidth = (int)lua_tointeger(L, 9);
    int SrcHeight = (int)lua_tointeger(L, 10);
    unsigned char Opacity = (unsigned char)lua_tointeger(L, 11);
    lua_pop(L, 11);
    PiCanvas::blend(gDest, gSrc, xDest, yDest, DestWidth, DestHeight,
        xSrc, ySrc, SrcWidth, SrcHeight, Opacity);
    return 0;
}

// lua: void PasteToWnd(WndGraphic, g)
int PasteToWnd(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    auto w = (GdiCanvas*)lua_tointeger(L, 1);
    auto g = (ImageMatrix*)lua_tointeger(L, 2);
    lua_pop(L, 2);
    w->paste(g, 0, 0);
    return 0;
}

// lua: void PasteToWndEx(WndGraphic, g, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight)
int PasteToWndEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 10) return 0;
    auto w = (GdiCanvas*)lua_tointeger(L, 1);
    auto g = (ImageMatrix*)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    int DestWidth = (int)lua_tointeger(L, 5);
    int DestHeight = (int)lua_tointeger(L, 6);
    int xSrc = (int)lua_tointeger(L, 7);
    int ySrc = (int)lua_tointeger(L, 8);
    int SrcWidth = (int)lua_tointeger(L, 9);
    int SrcHeight = (int)lua_tointeger(L, 10);
    lua_pop(L, 10);
    w->paste(g, xDest, yDest, DestWidth, DestHeight,
        xSrc, ySrc, SrcWidth, SrcHeight);
    return 0;
}

// lua: void StopSound(sound)
int StopSound(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    lua_pop(L, 1);
    if (BASS_ACTIVE_PLAYING == BASS_ChannelIsActive(sound))
        BASS_ChannelStop(sound);
    return 0;
}

// lua: void SetVolume(sound,volume), volume: 0-1
int SetVolume(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    float volume = (float)lua_tonumber(L, 2);
    lua_pop(L, 2);
    BASS_ChannelSetAttribute(sound, BASS_ATTRIB_VOL, volume);
    return 0;
}

// lua: void PlaySound(sound)
int PlaySound(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    lua_pop(L, 1);
    BASS_ChannelPlay(sound, true);
    return 0;
}

// lua: bool Screenshot(), true for success
int Screenshot(lua_State *L)
{
    std::filesystem::create_directory("screenshot");

    SYSTEMTIME SysTime;
    GetSystemTime(&SysTime);
    static UINT n_png = 0;
    
    TCHAR szFileName[120];
    swprintf_s(szFileName, L"screenshot\\%d-%d-%d-%d-%d-%d_%d.png",
        SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, n_png++);

    HDC tdc = GetDC(w.getHWND());
    HDC hdc = CreateCompatibleDC(NULL);
    auto size = w.getSize();
    HBITMAP hbmp = CreateCompatibleBitmap(tdc, size.first, size.second);
    HBITMAP hobmp = (HBITMAP)SelectObject(hdc, hbmp);
    // DO COPY
    BitBlt(hdc, 0, 0, size.first, size.second, tdc, 0, 0, SRCCOPY);
    ReleaseDC(w.getHWND(), tdc);

    BITMAP bmpObj;
    GetObject(hbmp, sizeof(BITMAP), &bmpObj);
    BITMAPINFOHEADER bmpHead;
    bmpHead.biBitCount = (WORD)(GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES));
    bmpHead.biCompression = BI_RGB;
    bmpHead.biPlanes = 1;
    bmpHead.biHeight = -bmpObj.bmHeight;
    bmpHead.biWidth = bmpObj.bmWidth;
    bmpHead.biSize = sizeof BITMAPINFOHEADER;
    // READ PIXEL DATA
    uint32_t* tmp = new uint32_t[bmpObj.bmWidth*bmpObj.bmHeight];
    GetDIBits(hdc, hbmp, 0, bmpObj.bmHeight, tmp, (LPBITMAPINFO)&bmpHead, DIB_RGB_COLORS);

    SelectObject(hdc, hobmp);
    DeleteObject(hbmp);
    DeleteObject(hobmp);
    DeleteDC(hdc);

    // DUMP TO FILE
    pImageMatrix im = ImageMatrixFactory::fromPixelData(tmp, bmpObj.bmWidth, bmpObj.bmHeight, true);
    ImageMatrixFactory::dumpPngFile(im, szFileName);

    lua_pushboolean(L, true);
    return 1;
}

// lua: string GetSetting(string key)
int GetSetting(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    std::string key = lua_tostring(L, 1);
    lua_pop(L, 1);
    try {
        std::string val = user_data.at(key);    // at() will throw std::out_of_range if not exists
        lua_pushlstring(L, val.c_str(), val.size());
    }
    catch (std::out_of_range oor) {
        lua_pushnil(L);    
    }
    return 1;
}

// lua: void SaveSetting(string key, string value)
int SaveSetting(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    std::string key = lua_tostring(L, 1);
    std::string val = lua_tostring(L, 2);
    user_data[key] = val;
    lua_pop(L, 2);
    return 0;
}

//==============================================
//=                  Callback                  =
//==============================================

bool LuaInit()
{
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

    extern size_t window_width, window_height;

    // Load configure
    bool ok = false;
    char* p = nullptr;
    uint32_t size;
    std::ifstream in;
    if (!bDirectMode && pack->readResource("./game/config.ini", &p, &size))
        ok = true;
    if (bDirectMode) {
        in.open("./game/config.ini", std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            p = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(p, size);
            ok = true;
        }
        in.close();
    }
    if (ok)
    {
        std::string conf_str(p, size);
        conf_str += '\n';

        // Get client size
        size_t result = conf_str.find("preferred_width", 0);
        if (result != std::string::npos) {
            size_t w_begin = 1 + conf_str.find('=', result);
            size_t w_end = conf_str.find('\n', w_begin);
            result = std::stoi(conf_str.substr(w_begin, w_end - w_begin));
            if (result > MIN_WIDTH) window_width = result;
        }
        result = conf_str.find("preferred_height", 0);
        if (result != std::string::npos) {
            size_t w_begin = 1 + conf_str.find('=', result);
            size_t w_end = conf_str.find('\n', w_begin);
            result = std::stoi(conf_str.substr(w_begin, w_end - w_begin));
            if (result > MIN_HEIGHT) window_height = result;
        }
        if (p) delete p;
    }
    ok = false;
    p = nullptr;
    if (!bDirectMode && pack->readResource("./game/lua/main.lua", &p, &size))
        ok = true;
    if (bDirectMode) {
        in.open("./game/lua/main.lua", std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            p = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(p, size);
            ok = true;
        }
        in.close();
    }
    // Load scripts & set screen size
    if (ok && luaL_loadbuffer(L, p, size, "line") == 0 &&
        lua_pcall(L, 0, LUA_MULTRET, 0) == 0)
    {
        lua_getglobal(L, "core");
        lua_pushinteger(L, window_width);
        lua_setfield(L, -2, "screenwidth");
        lua_getglobal(L, "core");
        lua_pushinteger(L, window_height);
        lua_setfield(L, -2, "screenheight");

        lua_getglobal(L, "OnCreate");
        lua_pcall(L, 0, 0, 0);

        if (p) delete p;
        return true;
    }
    else
    {
        w.alert(L"Fail to load main script!", L"Error", MB_ICONERROR);
        return false;
    }
}

void OnClean()
{
    BASS_Free();
    lua_close(L);
    L = nullptr;
}

// lua: void OnSetFocus()
void OnSetFocus() {
    lua_getglobal(L, "OnSetFocus");
    lua_pcall(L, 0, 0, 0);
}

// lua: void OnKillFocus()
void OnKillFocus() {
    if (L) {
        lua_getglobal(L, "OnKillFocus");
        lua_pcall(L, 0, 0, 0);
    }
}

// lua: void OnPaint(WndGraphic)
void OnPaint(const GdiCanvas& gdi)
{
    lua_getglobal(L, "OnPaint");
    lua_pushinteger(L, (lua_Integer)&gdi);
    lua_pcall(L, 1, 0, 0);
}

// lua: void OnClose()
void OnClose() {
    lua_getglobal(L, "OnClose");
    lua_pcall(L, 0, 0, 0);
}

// lua: void OnKeyDown(int key)
void OnKeyDown(int key) {
    lua_getglobal(L, "OnKeyDown");
    lua_pushinteger(L, key);
    lua_pcall(L, 1, 0, 0);
}

// lua: void OnKeyUp(int key)
void OnKeyUp(int key) {
    lua_getglobal(L, "OnKeyUp");
    lua_pushinteger(L, key);
    lua_pcall(L, 1, 0, 0);
}

// lua: void OnLButtonDown(int x,int y)
void OnLButtonDown(uint32_t, int x, int y) {
    lua_getglobal(L, "OnLButtonDown");
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pcall(L, 2, 0, 0);
}

// lua: void OnLButtonUP(int x,int y)
void OnLButtonUp(uint32_t, int x, int y) {
    lua_getglobal(L, "OnLButtonUp");
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pcall(L, 2, 0, 0);
}

// lua: void OnMouseMove(int x,int y)
void OnMouseMove(uint32_t, int x, int y) {
    lua_getglobal(L, "OnMouseMove");
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pcall(L, 2, 0, 0);
}

// lua: void OnMouseWheel(int zDeta, int x, int y)
void OnMouseWheel(uint32_t, short zDelta, int x, int y) {
    lua_getglobal(L, "OnMouseWheel");
    lua_pushinteger(L, zDelta);
    auto location = w.getPos();
    lua_pushinteger(L, x - location.first);
    lua_pushinteger(L, y - location.second);
    lua_pcall(L, 3, 0, 0);
}

