/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
    #include <windows.h>
    #undef max
#endif /* _WIN32 */
#ifdef _GTK
    #include <gtk/gtk.h>
#endif /* _GTK */
#include "Window.h"
#include "Keycodes.h"

#ifdef _WIN32
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
        callbackSource[hWnd]->getLButtonDownHandler()(
            (uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_LBUTTONUP:
        callbackSource[hWnd]->getLButtonUpHandler()(
            (uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_RBUTTONDOWN:
        callbackSource[hWnd]->getRButtonDownHandler()(
            (uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_RBUTTONUP:
        callbackSource[hWnd]->getRButtonUpHandler()(
            (uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;
    case WM_MOUSEMOVE:
        callbackSource[hWnd]->getMouseMoveHandler()(
            (uint32_t)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
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
    case WM_SETFOCUS:
        callbackSource[hWnd]->getGetFocusHandler()();
        break;
    case WM_KILLFOCUS:
        callbackSource[hWnd]->getLoseFocusHandler()();
        break;
    case WM_GETMINMAXINFO:
        MINMAXINFO* lpMMI;  // Minimum size
        lpMMI = (MINMAXINFO*)lParam;
        lpMMI->ptMinTrackSize.x = min_w;
        lpMMI->ptMinTrackSize.y = min_h;
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
    string_t wndClassName = title + L"cls";
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

void Window::alert(const char* str, const char* title, MessageType type)
{
    MessageBoxA(hWnd, str, title, type | MB_OK);
}

void Window::alert(const string_t& str, const string_t& title, MessageType type)
{
    MessageBoxW(hWnd, str.c_str(), title.c_str(), type | MB_OK);
}

void Window::setTitle(const string_t& str)
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
    width = (uint16_t)(rc.right - rc.left);
    height = (uint16_t)(rc.bottom - rc.top);
    return { width, height };
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
    Window w;
    w.argv = CommandLineToArgvW(lpCmdLine, &w.argc);
    w.create(L"demo");
}
#endif /* UNIT_TEST */

#endif /* _WIN32 */
#ifdef _GTK
gboolean keydown(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    w->getKeyDownHandler()(event->keyval);
    return TRUE;
}

gboolean keyup(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    w->getKeyUpHandler()(event->keyval);
    return TRUE;
}

gboolean btndown(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    uint32_t modifiers = 0;
    // GdkModifierType
    if (event->state & GDK_SHIFT_MASK)
        modifiers |= KEY_Shift_L;
    if (event->state & GDK_CONTROL_MASK)
        modifiers |= KEY_Control_L;
    if (event->state & GDK_MOD1_MASK)
        modifiers |= KEY_Alt_L;
    if (event->button == GDK_BUTTON_PRIMARY)
        w->getLButtonDownHandler()(modifiers, event->x, event->y);
    else if (event->button == GDK_BUTTON_SECONDARY)
        w->getRButtonDownHandler()(modifiers, event->x, event->y);
    if (event->type == GDK_2BUTTON_PRESS) 
        ; // double click
    return TRUE;
}

gboolean btnup(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    uint32_t modifiers = 0;
    // GdkModifierType
    if (event->state & GDK_SHIFT_MASK)
        modifiers |= KEY_Shift_L;
    if (event->state & GDK_CONTROL_MASK)
        modifiers |= KEY_Control_L;
    if (event->state & GDK_MOD1_MASK)
        modifiers |= KEY_Alt_L;
    if (event->button == GDK_BUTTON_PRIMARY)
        w->getLButtonUpHandler()(modifiers, event->x, event->y);
    else if (event->button == GDK_BUTTON_SECONDARY)
        w->getRButtonUpHandler()(modifiers, event->x, event->y);
    return TRUE;
}

gboolean mousemove(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    uint32_t modifiers = 0;
    // GdkModifierType
    if (event->state & GDK_SHIFT_MASK)
        modifiers |= KEY_Shift_L;
    if (event->state & GDK_CONTROL_MASK)
        modifiers |= KEY_Control_L;
    if (event->state & GDK_MOD1_MASK)
        modifiers |= KEY_Alt_L;
    w->getMouseMoveHandler()(modifiers, event->x, event->y);
    return TRUE;
}

gboolean mousewheel(GtkWidget *widget, GdkEventScroll *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    uint32_t modifiers = 0;
    // GdkModifierType
    if (event->state & GDK_SHIFT_MASK)
        modifiers |= KEY_Shift_L;
    if (event->state & GDK_CONTROL_MASK)
        modifiers |= KEY_Control_L;
    if (event->state & GDK_MOD1_MASK)
        modifiers |= KEY_Alt_L;
    if (event->direction == GDK_SCROLL_UP)
        w->getMouseWheelHandler()(modifiers,
            (short)event->delta_y, 
            (int)event->x_root, 
            (int)event->y_root);
    else if (event->direction == GDK_SCROLL_DOWN)
        w->getMouseWheelHandler()(modifiers, 
            (short)-event->delta_y, 
            (int)event->x_root, 
            (int)event->y_root);
    return TRUE;
}

gboolean paint(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    /*
     * Don't need to restore cr. The signal emission takes care of 
     * calling cairo_save() before and cairo_restore() after invoking 
     * the handler.
     * To avoid redrawing themselves completely, one can get the full 
     * extents of the clip region with cairo_copy_clip_rectangle_list().
     */
    Window *w = (Window *)user_data;
    GdiCanvas gdi(GTK_WINDOW(w->wnd), cr);
    w->getPaintHandler()(gdi);
    return TRUE;
}

gboolean focus(GtkWindow *window, GtkWidget *widget, gpointer user_data)
{
    Window *w = (Window *)user_data;
    if (widget == NULL)
        w->getLoseFocusHandler()();
    else
        w->getGetFocusHandler()();
    return TRUE;
}

gboolean onclose(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    Window *w = (Window *)user_data;
    return !w->getExitCallback()();
}

void activate(GtkApplication* app, gpointer user_data)
{
    Window *w = (Window *)user_data;
    w->wnd = gtk_application_window_new(app);
    // set window size and min max size
    gtk_window_set_default_size(GTK_WINDOW(w->wnd), w->width, w->height);
    GdkGeometry size_hints = {
        .min_width = w->min_w,
        .min_height = w->min_h,
        .max_width = w->width,
        .max_height = w->height,
        .base_width = 0,
        .base_height = 0,
        .width_inc = 0,
        .height_inc = 0,
        .min_aspect = 0.0,
        .max_aspect = 0.0,
        .win_gravity = GDK_GRAVITY_NORTH_WEST
    };
    gtk_window_set_geometry_hints(
        GTK_WINDOW(w->wnd), NULL, &size_hints,
        (GdkWindowHints)(GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE)
        );
    // set window title and position
    gtk_window_set_title(GTK_WINDOW(w->wnd), w->title.c_str());
    gtk_window_set_position(GTK_WINDOW(w->wnd), GTK_WIN_POS_CENTER);
    g_signal_connect(w->wnd, "set-focus", G_CALLBACK(focus), w);
    g_signal_connect(w->wnd, "delete-event", G_CALLBACK(onclose), w);

    // draw area events
    GtkWidget *draw_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(w->wnd), draw_area);
    gtk_widget_add_events(draw_area,
        GDK_POINTER_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK | 
        GDK_BUTTON_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK | 
        GDK_BUTTON_RELEASE_MASK | 
        GDK_KEY_PRESS_MASK | 
        GDK_KEY_RELEASE_MASK |
        GDK_SCROLL_MASK );
    g_signal_connect(draw_area, "key-press-event", G_CALLBACK(keydown), w);
    g_signal_connect(draw_area, "key-release-event", G_CALLBACK(keyup), w);
    g_signal_connect(draw_area, "button-press-event", G_CALLBACK(btndown), w);
    g_signal_connect(draw_area, "button-release-event", G_CALLBACK(btnup), w);
    g_signal_connect(draw_area, "motion-notify-event", G_CALLBACK(mousemove), w);
    g_signal_connect(draw_area, "scroll-event", G_CALLBACK(mousewheel), w);
    g_signal_connect(draw_area, "draw", G_CALLBACK(paint), w);

    gtk_widget_show_all(w->wnd);
}

void Window::looper()
{
    app = gtk_application_new("cc.moooc.window", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), this);
    g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
}

void Window::refresh()
{
    int w, h;
    gtk_window_get_size(GTK_WINDOW(wnd), &w, &h);
    gtk_widget_queue_draw_area(wnd, 0, 0, w, h);
}

void Window::alert(const char* str, const char* title, MessageType type)
{  
    /*
     * The message box icon may not display, don't know why.
     */
    if (app == nullptr)
    {
        // not initialized yet, just print the message
        fprintf(stderr, "%s: %s\n", title, str);
        return;
    }
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(wnd),
        GTK_DIALOG_MODAL,
        (GtkMessageType)type,
        GTK_BUTTONS_OK,
        str );
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void Window::alert(const string_t& str, const string_t& title, MessageType type)
{
    alert(str.c_str(), title.c_str(), type);
}

void Window::setTitle(const string_t& str)
{
    gtk_window_set_title(GTK_WINDOW(wnd), str.c_str());
}

std::pair<int, int> Window::getPos() const
{
    int x, y;
    gtk_window_get_position(GTK_WINDOW(wnd), &x, &y);
    return { x, y };
}

std::pair<int, int> Window::getSize() const
{
    int w, h;
    if (wnd)
    {
        gtk_window_get_size(GTK_WINDOW(wnd), &w, &h);
        if (w > 0 && w < UINT16_MAX && h > 0 && h < UINT16_MAX)
        {
            width = (uint16_t)w;
            height = (uint16_t)h;
        }
    }
    return { width, height };
}

#ifdef UNIT_TEST
int main(int argc, char* argv[])
{
    if (argc < 2)
        return -1;
    pImageMatrix im = ImageMatrixFactory::openImage(argv[1]);
    Window w;
    w.argc = argc;
    w.argv = argv;
    int x_pic=0, y_pic=0;
    w.setRButtonDownHandler([&](uint32_t vkeys, int x, int y) {
        // w.alert("hello", "good");
        x_pic = x;
        y_pic = y;
        w.refresh();
    });
    w.setKeyDownHandler([&w](int key) {
        if (key < 127) {
            char buf[2] = {0};
            buf[0] = (char)key;
            w.alert(buf, "you pressed:");
        }
    });
    int x1=0, y1=0;
    w.setLButtonDownHandler([&](uint32_t vkeys, int x, int y) {
        x1 = x;
        y1 = y;
    });
    int x2, y2;
    w.setMouseMoveHandler([&](uint32_t vkeys, int x, int y) {
        x2 = x;
        y2 = y;
        w.refresh();
    });
    w.setLButtonUpHandler([&](uint32_t vkeys, int x, int y) {
        w.refresh();
    });
    w.setPaintHandler([&](const GdiCanvas& gdi) {
        int width, height;
        std::tie(width, height) = w.getSize();
        gdi.fillSolidRect(0, 0, width, height, Canvas::Constant::yellow);
        gdi.drawLine(x1, y1, x2, y2, Canvas::LineStyle::none, Canvas::Constant::cyan);
        if (x_pic != 0 && y_pic != 0)
        {
            gdi.paste(im, x_pic-im->getWidth()*0.5, y_pic-im->getHeight()*0.5, 
                im->getWidth()*0.5, im->getHeight()*0.5, 
                0, 0, im->getWidth()/2, im->getHeight()/2
                );
            gdi.paste(im, x_pic, y_pic, im->getWidth()*0.5, im->getHeight()*0.5, 
                im->getWidth()/2, im->getHeight()/2, im->getWidth()/2, im->getHeight()/2
                );
        }
    });
    w.create("demo");
    return 0;
}
#endif /* UNIT_TEST */
#endif /* _GTK */
