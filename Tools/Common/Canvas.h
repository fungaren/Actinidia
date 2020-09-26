#pragma once
#include "ImageMatrix.h"
#include <string>

class Canvas
{
public:

    typedef uint8_t CharStyle;
    
    /**
     * 32bits color, format ARGB (0xAARRGGBB)
     * A: opacity, R: red, G: green, B: blue.
     */
    typedef uint32_t color;

    static inline uint8_t alphaChannel(color c) {
        return (uint8_t)(c >> 24);
    }
    static inline double alphaChannelf(color c) {
        return (uint8_t)(c >> 24) / 255.0;
    }
    static inline uint8_t redChannel(color c) {
        return (uint8_t)(c >> 16);
    }
    static inline double redChannelf(color c) {
        return (uint8_t)(c >> 16) / 255.0;
    }
    static inline uint8_t greenChannel(color c) {
        return (uint8_t)(c >> 8);
    }
    static inline double greenChannelf(color c) {
        return (uint8_t)(c >> 8) / 255.0;
    }
    static inline uint8_t blueChannel(color c) {
        return (uint8_t)c;
    }
    static inline double blueChannelf(color c) {
        return (uint8_t)c / 255.0;
    }
    static inline color rgb(uint8_t r, uint8_t g, uint8_t b) {
        return (0xFF << 24) | (r << 16) | (g << 8) | b;
    }
    struct Constant {
        static const color alpha    = 0x00FFFFFF;
        static const color white    = 0xFFFFFFFF;
        static const color black    = 0xFF000000;
        static const color red      = 0xFFFF0000;
        static const color green    = 0xFF00FF00;
        static const color blue     = 0xFF0000FF;
        static const color cyan     = 0xFF00FFFF;
        static const color magenta  = 0xFFFF00FF;
        static const color yellow   = 0xFFFFFF00;

        static const CharStyle style_default    = { 0 };
        static const CharStyle style_bold       = { 1 << 7 };
        static const CharStyle style_itatic     = { 1 << 6 };
        static const CharStyle style_underline  = { 1 << 5 };
    };

    /**
     * @param The ARGB format color.
     * @return The ABGR format color.
     */
    static color toABGR(color argb) {
        uint8_t b = (uint8_t)argb;
        uint8_t r = (uint8_t)(argb >> 16);
        argb &= 0xFF00FF00;
        return argb | (b << 16) | r;
    }

    enum LineStyle {
#ifdef _WIN32
        solid   = PS_SOLID,
        dashed  = PS_DASH,
        dotted  = PS_DOT,
        none    = PS_NULL
#endif /* _WIN32 */
#ifdef _GTK
        solid,
        dashed,
        dotted,
        none
#endif /* _WIN32 */
    };

    /**
     * @param dest color1
     * @param src color2
     * @param opacity blend color2 to color1 with opacity (0-255).
     * @return Color after blend color1 and color2. If opacity is 0
     *         return color1. If opacity is 255, return color2.
     */
    static color blend(color dest, color src, uint8_t opacity) {
#if 0
        uint8_t ch_d_alpha = dest >> 24;
        uint8_t ch_s_alpha = src >> 24;
        uint8_t ch_d_red = dest >> 16;
        uint8_t ch_s_red = src >> 16;
        uint8_t ch_d_green = dest >> 8;
        uint8_t ch_s_green = src >> 8;
        uint8_t ch_d_blue = dest;
        uint8_t ch_s_blue = src;
        
        color mixed_alpha = ch_s_alpha + ch_d_alpha;
        opacity = ch_s_alpha * opacity / 256;  // divide by 256 for better performance
        mixed_alpha = mixed_alpha > 255 ? 0xFF000000 : mixed_alpha << 24;

        color mixed_red = (ch_s_red * opacity + ch_d_red * (255 - opacity)) / 256;
        color mixed_green = (ch_s_green * opacity + ch_d_green * (255 - opacity)) / 256;
        color mixed_blue = (ch_s_blue * opacity + ch_d_blue * (255 - opacity)) / 256;
        return mixed_alpha | mixed_red << 16 | mixed_green << 8 | mixed_blue;
#else
        // Alpha
        uint32_t ch_src = src >> 24;
        uint32_t ch_dst = dest >> 24;
        opacity = ch_src * opacity / 256;  // divide by 256 for better performance
        color mixed = ch_src + ch_dst;
        mixed = mixed > 255 ? 0xFF00 : mixed << 8;
        // Red
        ch_src = (src >> 16) & 0xFF;
        ch_dst = (dest >> 16) & 0xFF;
        mixed |= (ch_src * opacity + ch_dst * (255 - opacity)) / 256;
        mixed <<= 8;
        // Green
        ch_src = (src >> 8) & 0xFF;
        ch_dst = (dest >> 8) & 0xFF;
        mixed |= (ch_src * opacity + ch_dst * (255 - opacity)) / 256;
        mixed <<= 8;
        // Blue
        ch_src = src & 0xFF;
        ch_dst = dest & 0xFF;
        mixed |= (ch_src * opacity + ch_dst * (255 - opacity)) / 256;
        return mixed;
#endif
    }
};

/**
 * @class PiCanvas
 * @brief Platform-independent Canvas
 */
class PiCanvas : Canvas
{
    PiCanvas();
public:
    /**
     * @param im ImageMatrix handle.
     * @param x x-coordinate.
     * @param y y-coordinate.
     * @return The color of the pixel at (x, y).
     */
    static color getPixel(const pImageMatrix im, int x, int y) noexcept(false);

    /**
     * @param im ImageMatrix handle.
     * @param left 
     * @param top 
     * @param right 
     * @param bottom 
     * @param fillColor Fill the rectangle with the color.
     */
    static void fillSolidRect(pImageMatrix im, int left, int top, int right, int bottom, color fillColor);

    /**
     * @param im ImageMatrix handle.
     * @param x The x-coordinate where to draw text.
     * @param y The y-coordinate where to draw text.
     * @param str The text need to draw.
     * @param len The length of text that need to draw.
     * @param fontName The font name used to draw text.
     * @param fontColor Draw text with the color.
     * @param style The text style (@ref style_default, @ref style_bold, 
     *        @ref style_itatic, @ref style_underline).
     */
    static void printText(pImageMatrix im, int x, int y, std::wstring str, uint16_t len,
        std::wstring fontName, uint8_t fontSize, color fontColor = Constant::black,
        CharStyle style = Constant::style_default) {
        return;
    }

    /**
     * @param im ImageMatrix handle.
     * @param x1 The x-coordinate of line start.
     * @param y1 The y-coordinate of line start.
     * @param x2 The x-coordinate of line end.
     * @param y2 The y-coordinate of line end.
     * @param ls The line style of the rectangle. Default is @ref solid.
     * @param lineColor Use the color to draw rectangle. Default is black.
     */
    static void drawLine(pImageMatrix im, int x1, int y1, int x2, int y2,
        LineStyle ls = LineStyle::solid, color lineColor = Constant::black);

    /**
     * @param im ImageMatrix handle.
     * @param left 
     * @param top 
     * @param right 
     * @param bottom 
     * @param ls The line style of the rectangle. Default is @ref solid.
     * @param lineColor Use the color to draw rectangle. Default is black.
     */
    static void rectangle(pImageMatrix im, int left, int top, int right, int bottom,
        LineStyle ls = LineStyle::solid, color lineColor = Constant::black);

    /**
     * @param imDest ImageMatrix handle of destination.
     * @param imSrc ImageMatrix handle of source.
     * @param xDest The x-coordinate where to paste.
     * @param yDest The y-coordinate where to paste.
     * @param opacity Opacity of imSrc to blend (0-255).
     */
    static void blend(pImageMatrix imDest, const pImageMatrix imSrc,
        int xDest, int yDest, uint8_t opacity);

    /**
     * @param imDest ImageMatrix handle of destination.
     * @param imSrc ImageMatrix handle of source.
     * @param xDest The x-coordinate where to paste.
     * @param yDest The y-coordinate where to paste.
     * @param destWidth The width of destinate paste area.
     * @param destHeight The height of destinate paste area.
     * @param xSrc The x-coordinate from where to copy.
     * @param ySrc The y-coordinate from where to copy.
     * @param srcWidth The width of original copy area.
     * @param srcHeight The height of original copy area.
     * @param opacity Opacity of imSrc to blend.
     */
    static void blend(pImageMatrix imDest, const pImageMatrix imSrc,
        int xDest, int yDest, int destWidth, int destHeight,
        int xSrc, int ySrc, int srcWidth, int srcHeight, uint8_t opacity);
};

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
#endif /* _WIN32 */
#ifdef _GTK
    cairo_t *cr;
    GtkWindow *wnd;
#endif /* _GTK */

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
#endif /* _WIN32 */
#ifdef _GTK
    GdiCanvas(GtkWindow *wnd, cairo_t *cr)
        :cr(cr), wnd(wnd)
    { }

    ~GdiCanvas() {}
#endif /* _GTK */
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
    //int calcHeight(const std::wstring& str, int width)
    //{
    //  RECT rc = { 0,0,width,0 };
    //  DrawTextW(hdc, str.c_str(), str.length(), &rc, format | DT_CALCRECT);
    //  return rc.bottom - rc.top;
    //}

    //Font& drawText(const std::wstring& str, int left, int top, int width, int height) {
    //  RECT rc = { left, top, left + width, top + height };

    //  SetBkMode(hdc, TRANSPARENT);
    //  SetTextColor(hdc, color);

    //  DrawTextW(hdc, str.c_str(), str.length(), &rc, format);
    //  return *this;
    //}

    bool printText(int x, int y, std::wstring str, uint16_t len,
        std::wstring fontName, uint8_t fontSize, color fontColor = Constant::black, 
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
