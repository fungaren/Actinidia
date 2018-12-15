#include "pch.h"
#include "Window.h"

#ifdef _WIN32

#include <map>
std::map<HWND, const Window*> callbackSource;

LRESULT CALLBACK dispacher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		callbackSource[hWnd]->getKeyDownHandler()(wParam);
		break;
	case WM_KEYUP:
		callbackSource[hWnd]->getKeyUpHandler()(wParam);
		break;
	case WM_LBUTTONDOWN:
		callbackSource[hWnd]->getLButtonDownHandler()(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		callbackSource[hWnd]->getLButtonUpHandler()(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		callbackSource[hWnd]->getRButtonDownHandler()(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		callbackSource[hWnd]->getRButtonUpHandler()(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		callbackSource[hWnd]->getMouseMoveHandler()(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		callbackSource[hWnd]->getMouseWheelHandler()(
			GET_WHEEL_DELTA_WPARAM(wParam),
			(int)LOWORD(lParam) - rc.left,
			(int)HIWORD(lParam) - rc.top
		);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		GdiCanvas gdi = GdiCanvas(hWnd, hdc);
		callbackSource[hWnd]->getPaintHandler()(gdi);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Window::looper() const
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = dispacher;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = title.c_str();
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);
	HWND hWnd = CreateWindowW(title.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return;
	callbackSource[hWnd] = this;
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

#endif
#ifdef _GTK

void Window::looper() const
{

}

#endif
