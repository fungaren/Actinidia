#pragma once
#include "Canvas.h"

class Window
{
    std::thread tWnd;
    std::wstring title;
    uint16_t width;
    uint16_t height;

#ifdef _WIN32
    HWND hWnd;
    void looper(HWND parent);
#endif /* _WIN32 */

#ifdef _GTK
    void looper();
#endif /* _GTK */

    // int key: The virtual-key code of the key when the ALT key is not pressed.
    std::function<void(int)> onKeyDown = [](int) {};
    std::function<void(int)> onKeyUp = [](int) {};
    // uint32 vkeys: Indicates whether various virtual keys are down.
    // int x: Specifies the x-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area.
    // int y: Specifies the y-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area.
    std::function<void(uint32_t, int, int)> onLButtonDown = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onLButtonUp = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onRButtonDown = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onRButtonUp = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onMouseMove = [](uint32_t, int, int) {};
    // uint32 vkeys: Indicates whether various virtual keys are down.
    // short zDelta: indicates the distance the wheel is rotated. A positive value indicates that the wheel was rotated forward,
    //               away from the user; a negative value indicates that the wheel was rotated backward, toward the user.
    // int x: Specifies the x - coordinate of the pointer, relative to the upper - left corner of the screen.
    // int y: Specifies the y - coordinate of the pointer, relative to the upper - left corner of the screen.
    std::function<void(uint32_t, short, int, int)> onMouseWheel = [](uint32_t, short, int, int) {};
    // GdiCanvas c: the Canvas you can draw stuff on
    std::function<void(const GdiCanvas&)> onPaint = [](const GdiCanvas&) {};
    // ret bool: indicate do exit or not
    std::function<bool()> onExit = [] { return true; };
#ifdef _WIN32
    // uint32_t message: A message id constant
    // ret bool: indicate whether any message handled
    std::function<bool(uint32_t, WPARAM, LPARAM)> onElse = [](uint32_t, WPARAM, LPARAM) { return false; };
#endif /* _WIN32 */

    void isRunning() {
        if (tWnd.joinable())
            throw std::runtime_error("Cannot set handler when the window is running");
    }

public:
    Window() {}
    Window(Window&) = delete;
    Window(Window&& w) = delete;
    void operator=(Window&) = delete;
    void operator=(Window&&) = delete;

    ~Window() {
        if (tWnd.joinable())
            tWnd.join();
    }
#ifdef _WIN32
    // If parent window is not set (default), Windows Messages will be take over.
    // Otherwise, you must call dispacher(...) manually in the parent WndProc.
    void create(const std::wstring& title, uint16_t width = 600, uint16_t height = 400,
        HWND parent=NULL) {
        if (title.empty())
            return;
        this->title = title;
        this->width = width;
        this->height = height;
        tWnd = std::thread(&Window::looper, this, parent);
    }
    static LRESULT CALLBACK dispacher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif /* _WIN32 */
#ifdef _GTK
    void create(const std::wstring& title, uint16_t width = 600, uint16_t height = 400) {
        if (title.empty())
            return;
        this->title = title;
        this->width = width;
        this->height = height;
        tWnd = std::thread(&Window::looper, this);
    }
#endif /* _GTK */
    void refresh();
    void alert(const std::string& str, const std::string& title, uint32_t flag = 0) const;
    void alert(const std::wstring& str, const std::wstring& title, uint32_t flag = 0) const;
    void setTitle(const std::wstring& str);

    // a pair (left, top)
    std::pair<int, int> getPos() const;
    // a pair (width, height)
    std::pair<int, int> getSize() const;

    template <typename T>
    void setKeyDownHandler(T&& handler)     { isRunning(); onKeyDown = handler; }
    template <typename T>
    void setKeyUpHandler(T&& handler)       { isRunning(); onKeyUp = handler; }
    template <typename T>
    void setLButtonDownHandler(T&& handler) { isRunning(); onLButtonDown = handler; }
    template <typename T>
    void setLButtonUpHandler(T&& handler)   { isRunning(); onLButtonUp = handler; }
    template <typename T>
    void setRButtonDownHandler(T&& handler) { isRunning(); onRButtonDown = handler; }
    template <typename T>
    void setRButtonUpHandler(T&& handler)   { isRunning(); onRButtonUp = handler; }
    template <typename T>
    void setMouseMoveHandler(T&& handler)   { isRunning(); onMouseMove = handler; }
    template <typename T>
    void setMouseWheelHandler(T&& handler)  { isRunning(); onMouseWheel = handler; }
    template <typename T>
    void setPaintHandler(T&& handler)       { isRunning(); onPaint = handler; }
    template <typename T>
    void setExitCallback(T&& handler)       { isRunning(); onExit = handler; }
#ifdef _WIN32
    template <typename T>
    void setElseHandler(T&& handler)        { isRunning(); onElse = handler; }
#endif /* _WIN32 */

    auto& getKeyDownHandler() const         { return onKeyDown; }
    auto& getKeyUpHandler() const           { return onKeyUp; }
    auto& getLButtonDownHandler() const     { return onLButtonDown; }
    auto& getLButtonUpHandler() const       { return onLButtonUp; }
    auto& getRButtonDownHandler() const     { return onRButtonDown; }
    auto& getRButtonUpHandler() const       { return onRButtonUp; }
    auto& getMouseMoveHandler() const       { return onMouseMove; }
    auto& getMouseWheelHandler() const      { return onMouseWheel; }
    auto& getPaintHandler() const           { return onPaint; }
    auto& getExitCallback() const           { return onExit; }
#ifdef _WIN32
    auto& getElseHandler() const            { return onElse; }
#endif /* _WIN32 */
};
