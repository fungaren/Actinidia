/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#pragma once
#include "Canvas.h"
/**
 * @class GdiCanvas
 * @brief Platform-dependent Canvas. For Windows, it is implemented by GDI.
 *        For Linux GTK+, it is implemented by Cairo.
 */
class GdiCanvas : Canvas
{
private:
#ifdef _WIN32
    HWND hwnd;
    HDC hdc;
#elif defined _GTK
    cairo_t *cr;
    GtkWindow *wnd;
#else
#error unsupported platform
#endif

public:

#ifdef _WIN32
    GdiCanvas(HWND hWnd)
        :hwnd(hWnd), hdc(GetDC(hWnd))
    { }

    GdiCanvas(HWND hWnd, HDC hdc)
        :hwnd(hWnd), hdc(hdc)
    { }

    ~GdiCanvas() {
        ReleaseDC(hwnd, hdc);
    }
#elif defined _GTK
    GdiCanvas(GtkWindow *wnd, cairo_t *cr)
        :cr(cr), wnd(wnd)
    { }

    ~GdiCanvas() {}
#else
#error unsupported platform
#endif

    GdiCanvas(GdiCanvas&) = delete;

    /**
     * @param left
     * @param top
     * @param right
     * @param bottom
     * @param fillColor Fill the rectangle with the color.
     */
    void fillSolidRect(int left, int top, int right, int bottom, color fillColor) const;

    //const unsigned int format = DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_NOCLIP | DT_HIDEPREFIX;
    //// calc expected height for specific text width
    //int calcHeight(const string_t& str, int width)
    //{
    //  RECT rc = { 0,0,width,0 };
    //  DrawText(hdc, str.c_str(), str.length(), &rc, format | DT_CALCRECT);
    //  return rc.bottom - rc.top;
    //}

    //Font& drawText(const string_t& str, int left, int top, int width, int height) {
    //  RECT rc = { left, top, left + width, top + height };

    //  SetBkMode(hdc, TRANSPARENT);
    //  SetTextColor(hdc, color);

    //  DrawText(hdc, str.c_str(), str.length(), &rc, format);
    //  return *this;
    //}

    bool printText(int x, int y, const string_t& str, uint16_t len,
        const string_t& fontName, uint8_t fontSize, color fontColor = Constant::black,
        CharStyle style = Constant::style_default) const;

    /**
     * @param x1 The x-coordinate of line start.
     * @param y1 The y-coordinate of line start.
     * @param x2 The x-coordinate of line end.
     * @param y2 The y-coordinate of line end.
     * @param ls The line style of the rectangle. Default is @ref solid.
     * @param lineColor Use the color to draw rectangle. Default is black.
     */
    void drawLine(int x1, int y1, int x2, int y2,
        LineStyle ls = LineStyle::solid, color lineColor = Constant::black) const;

    /**
     * @param left
     * @param top
     * @param right
     * @param bottom
     * @param ls The line style of the rectangle. Default is @ref solid.
     * @param lineColor Use the color to draw rectangle. Default is black.
     */
    void rectangle(int left, int top, int right, int bottom,
        LineStyle ls = LineStyle::solid, color lineColor = Constant::black) const;

    /**
     * @param imSrc ImageMatrix handle.
     * @param xDest The x-coordinate where to paste.
     * @param yDest The y-coordinate where to paste.
     * @note Do not support alpha blend. The alpha channel will be discarded.
     */
    void paste(const pImageMatrix imSrc, int xDest, int yDest) const;

    /**
     * @param imSrc ImageMatrix handle.
     * @param xDest The x-coordinate where to paste.
     * @param yDest The y-coordinate where to paste.
     * @param destWidth The width of destinate paste area.
     * @param destHeight The height of destinate paste area.
     * @param xSrc The x-coordinate from where to copy.
     * @param ySrc The y-coordinate from where to copy.
     * @param srcWidth The width of original copy area.
     * @param srcHeight The height of original copy area.
     * @note Do not support alpha blend. The alpha channel will be discarded.
     */
    void paste(const pImageMatrix imSrc,
        int xDest, int yDest, int destWidth, int destHeight,
        int xSrc, int ySrc, int srcWidth, int srcHeight) const;
};
