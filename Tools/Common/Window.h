#pragma once
#include "Canvas.h"
#include <thread>
#include <functional>
#include <codecvt>
#include <locale>

class Window
{
    std::thread tWnd;
    std::wstring title;
    mutable uint16_t width;
    mutable uint16_t height;

#ifdef _WIN32
    HWND hWnd;
    void looper(HWND parent, HICON icon);
#endif /* _WIN32 */
#ifdef _GTK
    GtkApplication *app;
    GtkWidget *wnd;
    void looper();
#endif /* _GTK */

    std::function<void(int)> onKeyDown = [](int) {};
    std::function<void(int)> onKeyUp = [](int) {};
    std::function<void(uint32_t, int, int)> onLButtonDown = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onLButtonUp = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onRButtonDown = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onRButtonUp = [](uint32_t, int, int) {};
    std::function<void(uint32_t, int, int)> onMouseMove = [](uint32_t, int, int) {};
    std::function<void(uint32_t, short, int, int)> onMouseWheel = [](uint32_t, short, int, int) {};
    std::function<void(const GdiCanvas&)> onPaint = [](const GdiCanvas&) {};
    std::function<bool()> onExit = [] { return true; };
#ifdef _WIN32
    std::function<bool(uint32_t, WPARAM, LPARAM)> onElse = [](uint32_t, WPARAM, LPARAM) { return false; };
#endif /* _WIN32 */
#ifdef _GTK
    /**
     * This signal is generated by GTK, and it will then create the window.
     * Declared as friend function to allow access to @ref wnd.
     */
    friend void activate(GtkApplication*, gpointer);
    friend gboolean paint(GtkWidget*, cairo_t*, gpointer);
#endif /* _GTK */
    void isRunning() {
        if (tWnd.joinable())
            throw std::runtime_error("Cannot set handler when the window is running");
    }

public:
    int argc;
#ifdef _WIN32
    wchar_t **argv;
    Window():
        width(0), height(0), hWnd(NULL), argc(0), argv(nullptr)
        {}
#endif /* _WIN32 */
#ifdef _GTK
    char **argv;
    Window():
        width(0), height(0), app(nullptr), wnd(nullptr), argc(0), argv(nullptr)
        {}
#endif /* _GTK */

    Window(Window&) = delete;
    Window(Window&& w) = delete;
    void operator=(Window&) = delete;
    void operator=(Window&&) = delete;

    ~Window() {
        if (tWnd.joinable())
            tWnd.join();
    }
#ifdef _WIN32
    /**
     * If parent window is not set (default), windows messages will be taken over.
     * Otherwise, you must call dispacher(...) manually in the parent WndProc.
     */
    void create(const std::wstring& title,
         uint16_t width = 600, uint16_t height = 400,
         HWND parent=NULL, HICON icon=NULL)
    {
        if (title.empty())
            return;
        this->title = title;
        this->width = width;
        this->height = height;
        tWnd = std::thread(&Window::looper, this, parent, icon);
    }
    static LRESULT CALLBACK dispacher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    // For some reason we need HWND
    HWND getHWND() { return hWnd; }
#endif /* _WIN32 */
#ifdef _GTK
    void create(const std::wstring& title,
         uint16_t width = 600, uint16_t height = 400)
    {
        if (title.empty())
            return;
        this->title = title;
        this->width = width;
        this->height = height;
        tWnd = std::thread(&Window::looper, this);
    }
#endif /* _GTK */
    /**
     * Invalidate the window so that the content will be redraw.
     */
    void refresh();

    /**
     * The type of message being displayed in the dialog.
     */
    enum MessageType
    {
#ifdef _WIN32
        INFO = WM_INFO,
        WARNING = WM_WARNING,
        QUESTION = WM_QUESTION,
        ERROR = WM_ERROR
#endif /* _WIN32 */
#ifdef _GTK
        INFO = GTK_MESSAGE_INFO,
        WARNING = GTK_MESSAGE_WARNING,
        QUESTION = GTK_MESSAGE_QUESTION,
        ERROR = GTK_MESSAGE_ERROR
#endif /* _GTK */
    };

    /** 
     * Create a message box to prompt something.
     * @param str The message that need to prompt.
     * @param title The title of the message box.
     * @param type The messagebox type, defined in @ref MessageType.
     */
    void alert(const char* str, 
               const char* title, MessageType type = INFO);
    void alert(const std::wstring& str,
               const std::wstring& title, MessageType type = INFO);
    void setTitle(const std::wstring& str);

    /**
     * @return The position (left, top) of the window.
     */
    std::pair<int, int> getPos() const;
    /**
     * @return The size (width, height) of the window. If the window is not 
     * created yet, default windows size is returned.
     */
    std::pair<int, int> getSize() const;

    /**
     * @param str A wide-char string.
     * @return A UTF-8 encoded C-type string.
     */
    const char* utf8(const std::wstring& str) const {
        static std::string s; // for temporary use
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
        s = cvt.to_bytes(title);
        return s.c_str();
    }

    /**
     * @param int key: The virtual-key code of the key when the ALT key is not pressed.
     */
    template <typename T>
    void setKeyDownHandler(T&& handler)     { isRunning(); onKeyDown = handler; }
    /**
     * @param int key: The virtual-key code of the key when the ALT key is not pressed.
     */
    template <typename T>
    void setKeyUpHandler(T&& handler)       { isRunning(); onKeyUp = handler; }
    /**
     * @param uint32 vkeys: Indicates whether various virtual keys are down.
     * @param int x: Specifies the x-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     * @param int y: Specifies the y-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     */
    template <typename T>
    void setLButtonDownHandler(T&& handler) { isRunning(); onLButtonDown = handler; }
    /**
     * @param uint32 vkeys: Indicates whether various virtual keys are down.
     * @param int x: Specifies the x-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     * @param int y: Specifies the y-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     */
    template <typename T>
    void setLButtonUpHandler(T&& handler)   { isRunning(); onLButtonUp = handler; }
    /**
     * @param uint32 vkeys: Indicates whether various virtual keys are down.
     * @param int x: Specifies the x-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     * @param int y: Specifies the y-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     */
    template <typename T>
    void setRButtonDownHandler(T&& handler) { isRunning(); onRButtonDown = handler; }
    /**
     * @param uint32 vkeys: Indicates whether various virtual keys are down.
     * @param int x: Specifies the x-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     * @param int y: Specifies the y-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     */
    template <typename T>
    void setRButtonUpHandler(T&& handler)   { isRunning(); onRButtonUp = handler; }
    /**
     * @param uint32 vkeys: Indicates whether various virtual keys are down.
     * @param int x: Specifies the x-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     * @param int y: Specifies the y-coordinate of the cursor. The coordinate is 
     *        relative to the upper-left corner of the client area.
     */
    template <typename T>
    void setMouseMoveHandler(T&& handler)   { isRunning(); onMouseMove = handler; }
    /**
     * @param uint32 vkeys: Indicates whether various virtual keys are down.
     * @param short zDelta: indicates the distance the wheel is rotated. A positive 
     *               value indicates that the wheel was rotated forward,
     *               away from the user; a negative value indicates that the 
     *               wheel was rotated backward, toward the user.
     * @param int x: Specifies the x - coordinate of the pointer, relative to the 
     *        upper - left corner of the screen.
     * @param int y: Specifies the y - coordinate of the pointer, relative to the 
     *        upper - left corner of the screen.
     */
    template <typename T>
    void setMouseWheelHandler(T&& handler)  { isRunning(); onMouseWheel = handler; }
    /**
     * @param const GdiCanvas& the Canvas you can draw stuff on
     */
    template <typename T>
    void setPaintHandler(T&& handler)       { isRunning(); onPaint = handler; }
    /**
     * @return false if you want to cancel the close.
     */
    template <typename T>
    void setExitCallback(T&& handler)       { isRunning(); onExit = handler; }
#ifdef _WIN32
    /**
     * @param uint32_t message: A message id constant
     * @return bool: indicate whether any message handled
     */
    template <typename T>
    void setElseHandler(T&& handler)        { isRunning(); onElse = handler; }
#endif /* _WIN32 */
#ifdef _GTK
#endif /* _GTK */
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
#ifdef _GTK

#endif /* _GTK */
};
