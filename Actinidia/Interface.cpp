/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
    #include <windows.h>
    #undef max
    #undef PlaySound

    #pragma comment(lib, "lua/lua.lib")
    #pragma comment(lib, "bass/bass.lib")

    #include "lua/lua.hpp"
#endif /* _WIN32 */
#ifdef _GTK
    #include <gtk/gtk.h>
    #include <lua5.3/lua.hpp>
    #include <iostream>
#endif /* _GTK */

#include <string>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include "../Tools/Common/Window.h"
#include "../Tools/Common/Canvas.h"
#include "../Tools/Common/ImageMatrix.h"
#include "../Tools/Common/ResourcePack.h"
#include "bass/bass.h"

lua_State* L = nullptr;
extern pResourcePack pack;
extern Window w;
extern bool bDirectMode;
extern std::map<const std::string, std::string> user_data;
extern std::map<size_t, pImageMatrix> im_handles;

extern size_t window_width, window_height;
extern const int MIN_WIDTH = 200;
extern const int MIN_HEIGHT = 100;


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
    try {
        auto g = ImageMatrixFactory::createBufferImage(width, height);
        auto key = std::hash<pImageMatrix>{}(g);
        lua_pushinteger(L, (lua_Integer)key);
        im_handles.insert({ key, g });
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "ERROR", Window::ERROR);
    }
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
    try {
        auto g = ImageMatrixFactory::createBufferImage(width, height, Canvas::color(c));
        auto key = std::hash<pImageMatrix>{}(g);
        lua_pushinteger(L, (lua_Integer)key);
        im_handles.insert({ key, g });
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "ERROR", Window::ERROR);
    }
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
    try {
        auto g = ImageMatrixFactory::createBufferImage(width, height, Canvas::Constant::alpha);
        auto key = std::hash<pImageMatrix>{}(g);
        lua_pushinteger(L, (lua_Integer)key);
        im_handles.insert({ key, g });
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "ERROR", Window::ERROR);
    }
    return 1;
}

// lua: void DeleteImage(g)
int DeleteImage(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    auto key = (size_t)lua_tointeger(L, 1);
    lua_pop(L, 1);
    im_handles.erase(key);
    return 0;
}

// lua: int GetWidth(g)
int GetWidth(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    auto key = (size_t)lua_tointeger(L, 1);
    lua_pop(L, 1);
    lua_pushinteger(L, im_handles[key]->getWidth());
    return 1;
}

// lua: int GetHeight(g)
int GetHeight(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    auto key = (size_t)lua_tointeger(L, 1);
    lua_pop(L, 1);
    lua_pushinteger(L, im_handles[key]->getHeight());
    return 1;
}

// lua: string GetText(pathname)
int GetText(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
    std::string f = lua_tostring(L, 1);
    lua_pop(L, 1);
    char* buffer;
    uint32_t size;
    if (bDirectMode) {
        std::ifstream in("./game/" + f, std::ios::binary);
        if (in.good()) {
            in.seekg(0, std::ios::end);
            size = (uint32_t)in.tellg();
            buffer = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(buffer, size);

            lua_pushlstring(L, buffer, size);
            return 1;
        }
    }
    else
    {
        if (pack->readResource("./game/" + f, &buffer, &size))
        {
            lua_pushlstring(L, buffer, size);
            return 1;
        }
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
    char* buffer;
    uint32_t size;
    if (bDirectMode) {
        try {
            pImageMatrix g = ImageMatrixFactory::openImage(("./game/" + f).c_str());
            auto key = std::hash<pImageMatrix>{}(g);
            lua_pushinteger(L, (lua_Integer)key);
            im_handles.insert({ key, g });
            return 1;
        }
        catch (std::runtime_error& e) {
            w.alert(e.what(), "ERROR", Window::ERROR);
        }
    }
    else
    {
        if (pack->readResource("./game/" + f, &buffer, &size))
        {
            try {
                pImageMatrix g;
                std::string extname = f.substr(f.size() - 3, 3);
                if (extname == "png" || extname == "PNG")
                    g = ImageMatrixFactory::fromPngBuffer(buffer, size);
                else
                    g = ImageMatrixFactory::fromJpegBuffer(buffer, size);
                auto key = std::hash<pImageMatrix>{}(g);
                lua_pushinteger(L, (lua_Integer)key);
                im_handles.insert({ key, g });
                return 1;
            }
            catch (std::runtime_error& e) {
                w.alert(e.what(), "ERROR", Window::ERROR);
            }
        }
    }
    lua_pushnil(L);
    return 1;
}

// lua: sound GetSound(pathname, b_loop), if b_loop set true, must StopSound() in OnClose()
int GetSound(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    std::string f = lua_tostring(L, 1);
    int b_loop = lua_toboolean(L, 2);
    lua_pop(L, 2);
    char* buffer;
    uint32_t size;
    if (bDirectMode) {
        std::ifstream in("./game/" + f, std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            buffer = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(buffer, size);

#ifdef _WIN32
            HSTREAM sound = BASS_StreamCreateFile(TRUE, buffer, 0, size,
                b_loop ? BASS_SAMPLE_LOOP : 0);
            if (sound)
            {
                lua_pushinteger(L, sound);
                return 1;
            }
#endif /* _WIN32 */
#ifdef _GTK
            HSTREAM sound = BASS_StreamCreateFile(TRUE, buffer, 0, size,
                b_loop ? BASS_SAMPLE_LOOP : 0);
            if (sound)
            {
                lua_pushinteger(L, sound);
                return 1;
            }
#endif /* _GTK */
        }
        in.close();
    }
    else
    {
        if (pack->readResource("./game/" + f, &buffer, &size))
        {
#ifdef _WIN32
            HSTREAM sound = BASS_StreamCreateFile(TRUE, buffer, 0, size,
                b_loop ? BASS_SAMPLE_LOOP : 0);
            if (sound)
            {
                lua_pushinteger(L, sound);
                return 1;
            }
#endif /* _WIN32 */
#ifdef _GTK
            HSTREAM sound = BASS_StreamCreateFile(TRUE, buffer, 0, size,
                b_loop ? BASS_SAMPLE_LOOP : 0);
            if (sound)
            {
                lua_pushinteger(L, sound);
                return 1;
            }
#endif /* _GTK */
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
    auto keyDest = (size_t)lua_tointeger(L, 1);
    auto keySrc = (size_t)lua_tointeger(L, 2);
    int x = (int)lua_tointeger(L, 3);
    int y = (int)lua_tointeger(L, 4);
    lua_pop(L, 4);
    try {
        PiCanvas::blend(im_handles[keyDest], im_handles[keySrc], x, y, 255);
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "ERROR", Window::ERROR);
    }
    return 0;
}

// lua: void PasteToImageEx(gDest, gSrc, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight)
int PasteToImageEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 10) return 0;
    auto keyDest = (size_t)lua_tointeger(L, 1);
    auto keySrc = (size_t)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    int DestWidth = (int)lua_tointeger(L, 5);
    int DestHeight = (int)lua_tointeger(L, 6);
    int xSrc = (int)lua_tointeger(L, 7);
    int ySrc = (int)lua_tointeger(L, 8);
    int SrcWidth = (int)lua_tointeger(L, 9);
    int SrcHeight = (int)lua_tointeger(L, 10);
    lua_pop(L, 10);
    try {
        PiCanvas::blend(im_handles[keyDest], im_handles[keySrc],
            xDest, yDest, DestWidth, DestHeight,
            xSrc, ySrc, SrcWidth, SrcHeight, 255);
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "Error", Window::ERROR);
    }
    return 0;
}

// lua: void AlphaBlend(gDest, gSrc, xDest, yDest, Opacity)
int AlphaBlend(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 5) return 0;
    auto keyDest = (size_t)lua_tointeger(L, 1);
    auto keySrc = (size_t)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    unsigned char Opacity = (unsigned char)lua_tointeger(L, 5);
    lua_pop(L, 5);
    try {
        PiCanvas::blend(im_handles[keyDest], im_handles[keySrc], xDest, yDest, Opacity);
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "Error", Window::ERROR);
    }
    return 0;
}

// lua: void AlphaBlendEx(gDest, gSrc, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight, Opacity)
int AlphaBlendEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 11) return 0;
    auto keyDest = (size_t)lua_tointeger(L, 1);
    auto keySrc = (size_t)lua_tointeger(L, 2);
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
    try {
        PiCanvas::blend(im_handles[keyDest], im_handles[keySrc], xDest, yDest, DestWidth, DestHeight,
            xSrc, ySrc, SrcWidth, SrcHeight, Opacity);
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "Error", Window::ERROR);
    }
    return 0;
}

// lua: void PasteToWnd(WndGraphic, g)
int PasteToWnd(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
    auto wnd = (GdiCanvas*)lua_tointeger(L, 1);
    auto key = (size_t)lua_tointeger(L, 2);
    lua_pop(L, 2);
    try {
        wnd->paste(im_handles[key], 0, 0);
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "Error", Window::ERROR);
    }
    return 0;
}

// lua: void PasteToWndEx(WndGraphic, g, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight)
int PasteToWndEx(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 10) return 0;
    auto wnd = (GdiCanvas*)lua_tointeger(L, 1);
    auto key = (size_t)lua_tointeger(L, 2);
    int xDest = (int)lua_tointeger(L, 3);
    int yDest = (int)lua_tointeger(L, 4);
    int DestWidth = (int)lua_tointeger(L, 5);
    int DestHeight = (int)lua_tointeger(L, 6);
    int xSrc = (int)lua_tointeger(L, 7);
    int ySrc = (int)lua_tointeger(L, 8);
    int SrcWidth = (int)lua_tointeger(L, 9);
    int SrcHeight = (int)lua_tointeger(L, 10);
    lua_pop(L, 10);
    try {
        wnd->paste(im_handles[key], xDest, yDest, DestWidth, DestHeight,
            xSrc, ySrc, SrcWidth, SrcHeight);
    } catch (std::runtime_error& e) {
        w.alert(e.what(), "Error", Window::ERROR);
    }
    return 0;
}

// lua: void StopSound(sound)
int StopSound(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
#ifdef _WIN32
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    lua_pop(L, 1);
    if (BASS_ACTIVE_PLAYING == BASS_ChannelIsActive(sound))
        BASS_ChannelStop(sound);
#endif /* _WIN32 */
#ifdef _GTK
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    lua_pop(L, 1);
    if (BASS_ACTIVE_PLAYING == BASS_ChannelIsActive(sound))
        BASS_ChannelStop(sound);
#endif /* _GTK */
    return 0;
}

// lua: void SetVolume(sound,volume), volume: 0-1
int SetVolume(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 2) return 0;
#ifdef _WIN32
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    float volume = (float)lua_tonumber(L, 2);
    lua_pop(L, 2);
    BASS_ChannelSetAttribute(sound, BASS_ATTRIB_VOL, volume);
#endif /* _WIN32 */
#ifdef _GTK
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    float volume = (float)lua_tonumber(L, 2);
    lua_pop(L, 2);
    BASS_ChannelSetAttribute(sound, BASS_ATTRIB_VOL, volume);
#endif /* _GTK */
    return 0;
}

// lua: void PlaySound(sound)
int PlaySound(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 1) return 0;
#ifdef _WIN32
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    lua_pop(L, 1);
    BASS_ChannelPlay(sound, true);
#endif /* _WIN32 */
#ifdef _GTK
    HSTREAM sound = (HSTREAM)lua_tointeger(L, 1);
    lua_pop(L, 1);
    BASS_ChannelPlay(sound, true);
#endif /* _GTK */
    return 0;
}

// lua: bool Screenshot(), true for success
int Screenshot(lua_State *L)
{
    static int n_png = 0;
    std::filesystem::create_directory("screenshot");
    std::time_t now = std::time(nullptr);

#ifdef _WIN32
    struct tm t;
    localtime_s(&t, &now);
    auto&& time = std::put_time(&t, L"%Y-%m-%d-%H-%M-%S");
    std::wstringstream ss;
    ss << L"screenshot\\" << time << L'_' << n_png++ << ".png";
#endif /* _WIN32 */
#ifdef _GTK
    auto&& time = std::put_time(std::localtime(&now), "%Y-%m-%d-%H-%M-%S");
    std::stringstream ss;
    ss << "screenshot/" << time << '_' << n_png++ << ".png";
    std::cout << "screenshot saved: " << ss.str() << '\n'; 
#endif /* _GTK */

    pImageMatrix im = w.getWindowImage();
    if (im)
    {
        ImageMatrixFactory::dumpPngFile(im, ss.str().c_str());
        lua_pushboolean(L, true);
    }
    else
        lua_pushboolean(L, false);
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
    catch (std::out_of_range& oor) {
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
#ifdef _WIN32
    BASS_Init(-1, 44100, 0, 0, 0);
#endif /* _WIN32 */
#ifdef _GTK
    BASS_Init(-1, 44100, 0, 0, 0);
#endif /* _GTK */

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

    // Load configure
    bool ok = false;
    char* buffer = nullptr;
    uint32_t size;
    std::ifstream in;
    if (!bDirectMode && pack->readResource("./game/config.ini", &buffer, &size))
        ok = true;
    if (bDirectMode) {
        in.open("./game/config.ini", std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            buffer = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(buffer, size);
            ok = true;
        }
        in.close();
    }
    if (ok)
    {
        std::string conf_str(buffer, size);
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
        delete[] buffer;
    }
    ok = false;
    buffer = nullptr;
    if (!bDirectMode && pack->readResource("./game/lua/main.lua", &buffer, &size))
        ok = true;
    if (bDirectMode) {
        in.open("./game/lua/main.lua", std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            buffer = new char[size];
            in.seekg(0, std::ios::beg);
            in.read(buffer, size);
            ok = true;
        }
        in.close();
    }
    // Load scripts & set screen size
    if (ok && luaL_loadbuffer(L, buffer, size, "line") == 0 &&
        lua_pcall(L, 0, LUA_MULTRET, 0) == 0)
    {
        lua_getglobal(L, "core");
        lua_pushinteger(L, window_width);
        lua_setfield(L, -2, "screenwidth");
        lua_getglobal(L, "core");
        lua_pushinteger(L, window_height);
        lua_setfield(L, -2, "screenheight");

        lua_getglobal(L, "OnCreate");
        lua_pcall(L, 0, 0, 0); // call OnCreate

        delete[] buffer;
        return true;
    }
    else
    {
        w.alert("Fail to load main script!", "Error", Window::ERROR);
        return false;
    }
}

void OnClean()
{
#ifdef _WIN32
    BASS_Free();
#endif /* _WIN32 */
#ifdef _GTK
    BASS_Free();
#endif /* _GTK */
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

