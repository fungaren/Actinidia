#pragma once

#include "resource.h"
#include "..\\ActinidiaMapEditor\\ActinidiaGraphic.hpp"
#include "lua\\lua.hpp"
#pragma comment(lib, "lua\\lua.lib")
#include <thread>
#include <atomic>
#include <map>
#include <string>
#include <fstream>

std::map<const std::string, std::string> prop;	// user data cache

extern int WIN_WIDTH;
extern int WIN_HEIGHT;
lua_State* L;
LPCSTR STR_ERR_TITLE = "ERROR";
LPCSTR STR_ERR_WRONG_RETVAL = "Wrong return type!";

// lua: string OnCreate(), "" for success, msg for error
inline int OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnCreate");
	if (lua_pcall(L, 0, 1, 0))
		return 0;		// 0 for success 
	else if (lua_isstring(L,-1))
	{
		const char* luaErr = lua_tostring(L, -1);
		if (*luaErr == 0)
			return 0;	// 0 for success
		else
		{
			MessageBoxA(hWnd, luaErr, STR_ERR_TITLE, MB_ICONEXCLAMATION);
			return 1;	// 1 for error
		}
	}
	else
	{
		MessageBoxA(hWnd, STR_ERR_WRONG_RETVAL, STR_ERR_TITLE, MB_ICONEXCLAMATION);
		return 1;		// 1 for error
	}
}

// lua: void OnSetFocus()
inline int OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnSetFocus");
	lua_pcall(L, 0, 0, 0);
	return 0;
}

// lua: void OnKillFocus()
inline int OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnKillFocus");
	lua_pcall(L, 0, 0, 0);
	return 0;
}

std::atomic<int> bTimerOpen = false;
bool bByTimer = false;
std::thread _timer;
static unsigned frame_count = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> fpsBegin;

void TimerProc(HWND hWnd)
{
	static RECT wndrect = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
	while (bTimerOpen)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(16667L));
		if (bTimerOpen) InvalidateRect(hWnd, &wndrect, FALSE);	// Repaint now		
	}
}

// lua: void OnPaint(WndGraphic)
inline int OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (bByTimer){
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		{
			auto w = new DeviceContextGraphic(hWnd, hdc);
			lua_getglobal(L, "OnPaint");
			lua_pushinteger(L, (DWORD)(void*)w);
			lua_pcall(L, 1, 0, 0);
			delete w;
		}
		EndPaint(hWnd, &ps);
		if (++frame_count % 15 == 0)	// Update fps per 15 frame
		{
			TCHAR buff[32];
			auto fpsNow = std::chrono::high_resolution_clock::now();
			swprintf(buff, 32, _T("fps:%.3lf"), 15.0*1000.0 / (double)
				std::chrono::duration_cast<std::chrono::milliseconds>(fpsNow - fpsBegin).count());
			fpsBegin = fpsNow;
			SetWindowText(hWnd, buff);
			frame_count = 1;			// Recount
		}
	}
	else if (!bTimerOpen)
	{
		bTimerOpen = true;
		bByTimer = true;
		fpsBegin = std::chrono::high_resolution_clock::now();
		_timer = std::thread(TimerProc, hWnd);
		// InvalidateRect(hWnd, &wndrect, FALSE);
		// UpdateWindow(hWnd);
	}
	return 0;
}

// lua: void OnClose()
inline int OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnClose");

	bTimerOpen = false;	// Close for a while
	_timer.join();
	lua_pcall(L, 0, 0, 0);
	return 0;
}

// lua: void OnKeyDown(int nChar)
inline int OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnKeyDown");
	lua_pushinteger(L, wParam);
	lua_pcall(L, 1, 0, 0);
	return 0;
}

// lua: void OnKeyUp(int nChar)
inline int OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnKeyUp");
	lua_pushinteger(L, wParam);
	lua_pcall(L, 1, 0, 0);
	return 0;
}

// lua: void OnLButtonDown(int x,int y)
inline int OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnLButtonDown");
	lua_pushinteger(L, (int)LOWORD(lParam));	// x
	lua_pushinteger(L, (int)HIWORD(lParam));	// y
	lua_pcall(L, 2, 0, 0);
	return 0;
}

// lua: void OnLButtonUP(int x,int y)
inline int OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnLButtonUp");
	lua_pushinteger(L, (int)LOWORD(lParam));	// x
	lua_pushinteger(L, (int)HIWORD(lParam));	// y
	lua_pcall(L, 2, 0, 0);
	return 0;
}

// lua: void OnMouseMove(int x,int y)
inline int OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	lua_getglobal(L, "OnMouseMove");
	lua_pushinteger(L, (int)LOWORD(lParam));	// x
	lua_pushinteger(L, (int)HIWORD(lParam));	// y
	lua_pcall(L, 2, 0, 0);
	return 0;
}

// lua: void OnMouseWheel(int zDeta, int x, int y)
inline int OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam){
	lua_getglobal(L, "OnMouseWheel");
	lua_pushinteger(L, LOWORD(wParam)/WHEEL_DELTA);
	RECT rc;
	GetClientRect(hWnd, &rc);
	lua_pushinteger(L, (int)LOWORD(lParam)-rc.left);	// x
	lua_pushinteger(L, (int)HIWORD(lParam)-rc.top);		// y
	lua_pcall(L, 3, 0, 0);
	return 0;
}