#include "stdafx.h"
#include "ActinidiaInterface4lua.h"

bool InitApp(LPCTSTR lpCmdLine)
{
	if (0 == lstrcmp(lpCmdLine, _T("DirectMode")))
	{
		bDirectMode = true;
	}
	else if (!InitLoader())
	{
		MessageBoxA(NULL, "Fail to load resources!", STR_ERR_TITLE, MB_ICONERROR);
		return false;
	}

	// load user data
	std::ifstream in(bDirectMode?"res\\data":"data", std::ios::in);
	char buff[1024];
	if (in.good()) {
		while (in.getline(buff, 1024)) {
			if (buff[0] == '#') continue;			// # this is comment
			const char* pos = strchr(buff, '=');	// eg. best = 999
			if (pos != NULL) {
				std::string key(buff, pos - buff);
				prop[key] = std::string(pos+1);
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
	for each (std::pair<const std::string, std::string> p in prop) {
		out << p.first.c_str() << '=' << p.second.c_str() << '\n';
	}
	out.close();
}

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		if (OnCreate(hWnd, wParam, lParam)){
			DestroyWindow(hWnd);
			return 0;
		}
		break;
	case WM_SETFOCUS:
		OnSetFocus(hWnd, wParam, lParam);
		break;
	case WM_KILLFOCUS:
		OnKillFocus(hWnd, wParam, lParam);
		break;
	case WM_PAINT:
		OnPaint(hWnd, wParam, lParam);
		break;
	case WM_CLOSE:
		OnClose(hWnd, wParam, lParam);
		break;
	case WM_KEYDOWN:
		OnKeyDown(hWnd, wParam, lParam);
		break;
	case WM_KEYUP:
		OnKeyUp(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		CleanAll();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(wcx);				// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// redraw if size changes 
	wcx.lpfnWndProc = MyWndProc;			// points to window procedure 
	wcx.cbClsExtra = 0;						// no extra class memory 
	wcx.cbWndExtra = 0;						// no extra window memory
	wcx.hInstance = hInst;					// handle to instance
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = NULL;				// no background brush
	wcx.lpszMenuName = NULL;				// name of menu resource
	wcx.lpszClassName = L"MainWClass";		// name of window class
	wcx.hIconSm = NULL;

	if (!RegisterClassEx(&wcx))
		return FALSE;

	if (!InitApp(lpCmdLine))	// Scripts start working!
		return FALSE;

	RECT rc;
	SetRect(&rc, 0, 0, WIN_WIDTH, WIN_HEIGHT);			// Client size
	DWORD grfStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	AdjustWindowRect(&rc, grfStyle, FALSE);

	hwnd = CreateWindow(
		L"MainWClass",			// name of window class 
		L"ActinidiaGo",			// title-bar string 
		grfStyle,
		10,					// default horizontal position 
		20,					// default vertical position 
		rc.right - rc.left,
		rc.bottom - rc.top,		// default height 
		(HWND)NULL,				// no owner window 
		(HMENU)NULL,			// no class menu 
		hInst,					// handle to application instance 
		(LPVOID)NULL);			// no window-creation data 

	if (!hwnd)
		return FALSE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, hwnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
