/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#include "Window.h"

#ifdef _WIN32
#include "pch.h"
#include <map>
std::map<HWND, const Window*> callbackSource;

LRESULT CALLBACK Window::dispacher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (callbackSource.find(hWnd) == callbackSource.end())
        return DefWindowProc(hWnd, message, wParam, lParam);    // not registered
    switch (message)
    {
    case WM_KEYDOWN:
        callbackSource[hWnd]->getKeyDownHandler()((uint32_t)wParam);
        break;
    case WM_KEYUP:
        callbackSource[hWnd]->getKeyUpHandler()((uint32_t)wParam);
        break;
    case WM_LBUTTONDOWN:
        callbackSource[hWnd]->getLButtonDownHandler()((uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_LBUTTONUP:
        callbackSource[hWnd]->getLButtonUpHandler()((uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_RBUTTONDOWN:
        callbackSource[hWnd]->getRButtonDownHandler()((uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_RBUTTONUP:
        callbackSource[hWnd]->getRButtonUpHandler()((uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_MOUSEMOVE:
        callbackSource[hWnd]->getMouseMoveHandler()((uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_MOUSEWHEEL:
        callbackSource[hWnd]->getMouseWheelHandler()(
            GET_KEYSTATE_WPARAM(wParam),
            GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA,
            (short)LOWORD(lParam),
            (short)HIWORD(lParam)
        );
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        GdiCanvas gdi(hWnd, hdc);
        callbackSource[hWnd]->getPaintHandler()(gdi);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CLOSE:
        if (callbackSource[hWnd]->getExitCallback()())
            return DefWindowProc(hWnd, message, wParam, lParam);
        else
            break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        if (!callbackSource[hWnd]->getElseHandler()(message, wParam, lParam))
            return DefWindowProc(hWnd, message, wParam, lParam); }
    return 0;
}

void Window::looper(HWND parent, HICON icon)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    std::wstring wndClassName = title + L"cls";
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = dispacher;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = icon;
    wcex.hIconSm = icon;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = wndClassName.c_str();
    RegisterClassExW(&wcex);
    this->hWnd = CreateWindowW(wndClassName.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, width, height, parent, NULL, hInstance, NULL);
    if (!hWnd) {
        int x = GetLastError();
        _ASSERT(0);
    }
    callbackSource[hWnd] = this;
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::refresh()
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    InvalidateRect(hWnd, &rc, FALSE);
}

void Window::alert(const std::string& str, const std::string& title, uint32_t flag) const
{
    MessageBoxA(hWnd, str.c_str(), title.c_str(), flag);
}

void Window::alert(const std::wstring& str, const std::wstring& title, uint32_t flag) const
{
    MessageBox(hWnd, str.c_str(), title.c_str(), flag);
}

void Window::setTitle(const std::wstring& str)
{
    SetWindowText(hWnd, str.c_str());
}

std::pair<int, int> Window::getPos() const
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    return { rc.left, rc.top };
}

std::pair<int, int> Window::getSize() const
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    return { rc.right - rc.left, rc.bottom - rc.top };
}

#ifdef UNIT_TEST
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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    Window w;
    w.create(L"demo");
}
#endif /* UNIT_TEST */

#endif /* _WIN32 */
#ifdef _GTK
#include <gtk/gtk.h>

#ifdef UNIT_TEST
static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget *window;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
    gtk_widget_show_all (window);
}

void Window::looper()
{

}

void Window::refresh()
{

}

int main(int argc, char* argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
#endif /* UNIT_TEST */
#endif /* _GTK */
