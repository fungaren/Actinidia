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
#include <fstream>
#include "ImageMatrix.h"
#include "GdiCanvas.h"

#ifdef _WIN32
void GdiCanvas::fillSolidRect(int left, int top, int right, int bottom, color fillColor) const
{
    SetBkColor(hdc, toABGR(fillColor) & 0x00FFFFFF);
    RECT rect{ left, top, right, bottom };
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}

bool GdiCanvas::printText(int x, int y, std::wstring str, uint16_t len,
    std::wstring fontName, uint8_t fontSize, color fontColor, CharStyle style) const
{
    uint32_t font_weight = style & Constant::style_bold ? 700 : 400;
    HFONT fn = CreateFont(fontSize, 0, 0, 0, font_weight, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS, fontName.c_str());
    HFONT oldfn = (HFONT)SelectObject(hdc, fn);
    SetTextColor(hdc, toABGR(fontColor) & 0x00FFFFFF);
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, x, y, str.c_str(), len);
    SelectObject(hdc, oldfn);
    DeleteObject(fn);
    DeleteObject(oldfn);
    return true;
}

void GdiCanvas::rectangle(int left, int top, int right, int bottom, LineStyle ls, color lineColor) const
{
    HGDIOBJ br = GetStockObject(NULL_BRUSH);
    HBRUSH Oldbr = (HBRUSH)SelectObject(hdc, br);

    HPEN pen = CreatePen(ls, 1, toABGR(lineColor) & 0x00FFFFFF);
    HPEN Oldpen = (HPEN)SelectObject(hdc, (HGDIOBJ)pen);

    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, Oldbr);
    SelectObject(hdc, Oldpen);
    DeleteObject(br);
    DeleteObject(pen);
    DeleteObject(Oldbr);
    DeleteObject(Oldpen);
}

void GdiCanvas::drawLine(int x1, int y1, int x2, int y2, LineStyle ls, color lineColor) const
{
    HPEN pen = CreatePen(ls, 1, toABGR(lineColor) & 0x00FFFFFF);
    HPEN Oldpen = (HPEN)SelectObject(hdc, (HGDIOBJ)pen);

    MoveToEx(hdc, x1, y1, 0);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, Oldpen);
    DeleteObject(pen);
    DeleteObject(Oldpen);
}

void GdiCanvas::paste(const pImageMatrix imSrc, int xDest, int yDest) const
{
    if (imSrc->getMatrix() == nullptr)
        throw std::invalid_argument("ImageMatrix is not initialized");
    int yRange, yStart;
    if (yDest < 0) {
        yRange = yDest + imSrc->getHeight();
        yStart = -yDest;
        yDest = 0;
    }
    else {
        yRange = imSrc->getHeight();
        yStart = 0;
    }

    int xRange, xStart;
    if (xDest < 0) {
        xRange = xDest + imSrc->getWidth();
        xStart = -xDest;
        xDest = 0;
    }
    else {
        xRange = imSrc->getWidth();
        xStart = 0;
    }

    uint32_t size = xRange * yRange;
    color* buffer = new color[size];
    uint32_t i = 0;
    for (int y = 0; y < yRange; ++y)
        for (int x = 0; x < xRange; ++x)
            buffer[i++] = _color(imSrc, x + xStart, y + yStart);

    BITMAPINFOHEADER bmih;
    bmih.biBitCount = 32;
    bmih.biClrImportant = 0;
    bmih.biClrUsed = 0;
    bmih.biCompression = BI_RGB;
    bmih.biWidth = xRange;
    bmih.biHeight = -yRange;    // top-down image 
    bmih.biPlanes = 1;
    bmih.biSize = 40;
    bmih.biSizeImage = size * sizeof(color);
    BITMAPINFO bmpi = { 0 };
    bmpi.bmiHeader = bmih;

    SetDIBitsToDevice(hdc, //handle to DC
        xDest,  // x-coord of destination upper-left corner
        yDest,  // y-coord of destination upper-left corner 
        xRange, // source rectangle width
        yRange, // source rectangle height
        0,      // x-coord of source lower-left corner (bmih.biHeight => upper-left)
        0,      // y-coord of source lower-left corner (bmih.biHeight => upper-left)
        0,      // first scan line in array
        yRange, // number of scan lines
        buffer, // array of DIB bits
        &bmpi,  // bitmap information
        DIB_RGB_COLORS // RGB or palette indexes
        );
    delete[] buffer;
}

void GdiCanvas::paste(const pImageMatrix imSrc,
    int xDest, int yDest, int destWidth, int destHeight,
    int xSrc, int ySrc, int srcWidth, int srcHeight) const
{
    if (imSrc->getMatrix() == nullptr)
        throw std::invalid_argument("ImageMatrix is not initialized");

    if (xSrc < 0 || ySrc < 0)
        return;
    if (ySrc + srcHeight > imSrc->getHeight() || xSrc + srcWidth > imSrc->getWidth())
        return;

    int yRange, yStart;
    if (yDest < 0) {
        yRange = yDest + destHeight;
        yStart = -yDest;
        yDest = 0;
    }
    else {
        yRange = destHeight;
        yStart = 0;
    }

    int xRange, xStart;
    if (xDest < 0) {
        xRange = xDest + destWidth;
        xStart = -xDest;
        xDest = 0;
    }
    else {
        xRange = destWidth;
        xStart = 0;
    }
    
    uint32_t size = xRange * yRange;
    color* buffer = new color[size];
    uint32_t i = 0;

    if (srcWidth == destWidth && srcHeight == destHeight)
        for (int y = 0; y < yRange; ++y)
            for (int x = 0; x < xRange; ++x)
                buffer[i++] = _color(imSrc, x + xSrc + xStart, y + ySrc + yStart);
    else
        for (int y = 0; y < yRange; ++y)
            for (int x = 0; x < xRange; ++x)
                buffer[i++] = _color(imSrc,
                    x*srcWidth / destWidth + xSrc + xStart,
                    y*srcHeight / destHeight + ySrc + yStart);

    BITMAPINFOHEADER bmih;
    bmih.biBitCount = 32;
    bmih.biClrImportant = 0;
    bmih.biClrUsed = 0;
    bmih.biCompression = BI_RGB;
    bmih.biWidth = xRange;
    bmih.biHeight = -yRange;    // top-down image 
    bmih.biPlanes = 1;
    bmih.biSize = 40;
    bmih.biSizeImage = size * sizeof(color);
    BITMAPINFO bmpi = { 0 };
    bmpi.bmiHeader = bmih;

    SetDIBitsToDevice(hdc, //handle to DC
        xDest,  // x-coord of destination upper-left corner
        yDest,  // y-coord of destination upper-left corner 
        xRange, // source rectangle width
        yRange, // source rectangle height
        0,      // x-coord of source lower-left corner (bmih.biHeight => upper-left)
        0,      // y-coord of source lower-left corner (bmih.biHeight => upper-left)
        0,      // first scan line in array
        yRange, // number of scan lines
        buffer, // array of DIB bits
        &bmpi,  // bitmap information
        DIB_RGB_COLORS // RGB or palette indexes
        );
    delete[] buffer;
}
#endif /* _WIN32 */
#ifdef _GTK
void GdiCanvas::fillSolidRect(int left, int top, int right, int bottom, color c) const
{
    cairo_set_source_rgb(cr, redChannelf(c), greenChannelf(c), blueChannelf(c));
    cairo_rectangle(cr, left, top, right-left, bottom-top);
    cairo_fill(cr);
}

bool GdiCanvas::printText(int x, int y, std::wstring str, uint16_t len,
    std::wstring fontName, uint8_t fontSize, color fontColor, 
    CharStyle style) const
{
    return true;
}

void GdiCanvas::drawLine(int x1, int y1, int x2, int y2,
    LineStyle ls, color c) const
{
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_SUBPIXEL);
    cairo_set_source_rgb(cr, redChannelf(c), greenChannelf(c), blueChannelf(c));
    cairo_set_line_width(cr, 1);
    /* cairo_set_dash */
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_stroke(cr);
}

void GdiCanvas::rectangle(int left, int top, int right, int bottom,
    LineStyle ls, color c) const
{
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_SUBPIXEL);
    cairo_set_source_rgb(cr, redChannelf(c), greenChannelf(c), blueChannelf(c));
    cairo_set_line_width(cr, 1);
    /* cairo_set_dash */
    cairo_rectangle(cr, left, top, right-left, bottom-top);
    cairo_stroke(cr); 
}

// Do not support alpha blend. The alpha channel will be discarded.
void GdiCanvas::paste(const pImageMatrix imSrc, int xDest, int yDest) const
{
    if (imSrc->getMatrix() == nullptr)
        throw std::invalid_argument("ImageMatrix is not initialized");
    int yRange, yStart;
    if (yDest < 0) {
        yRange = yDest + imSrc->getHeight();
        yStart = -yDest;
        yDest = 0;
    }
    else {
        yRange = imSrc->getHeight();
        yStart = 0;
    }

    int xRange, xStart;
    if (xDest < 0) {
        xRange = xDest + imSrc->getWidth();
        xStart = -xDest;
        xDest = 0;
    }
    else {
        xRange = imSrc->getWidth();
        xStart = 0;
    }

    uint32_t size = xRange * yRange;
    color* buffer = new color[size];
    uint32_t i = 0;
    for (int y = 0; y < yRange; ++y)
        for (int x = 0; x < xRange; ++x)
            buffer[i++] = _color(imSrc, x + xStart, y + yStart);

    cairo_format_t format = CAIRO_FORMAT_ARGB32;
    int stride = cairo_format_stride_for_width(format, xRange);
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        (uint8_t*)buffer, // a pointer to a buffer supplied by the application
        format, // the format of pixels in the buffer
        xRange, // the width of the image to be stored in the buffer
        yRange, // the height of the image to be stored in the buffer
        stride  // the number of bytes between the start of rows in the buffer as allocated.
    );
    cairo_set_source_surface(cr, surface, xDest, yDest);
    
    cairo_rectangle(cr, xDest, yDest, xRange, yRange);
    cairo_fill(cr);
    cairo_stroke(cr); 

    cairo_surface_finish(surface);
    // cairo_surface_destroy(surface)

    delete[] buffer;
}

// Do not support alpha blend. The alpha channel will be discarded.
void GdiCanvas::paste(const pImageMatrix imSrc,
    int xDest, int yDest, int destWidth, int destHeight,
    int xSrc, int ySrc, int srcWidth, int srcHeight) const
{
    if (imSrc->getMatrix() == nullptr)
        throw std::invalid_argument("ImageMatrix is not initialized");

    if (xSrc < 0 || ySrc < 0)
        return;
    if (ySrc + srcHeight > imSrc->getHeight() || xSrc + srcWidth > imSrc->getWidth())
        return;

    int yRange, yStart;
    if (yDest < 0) {
        yRange = yDest + destHeight;
        yStart = -yDest;
        yDest = 0;
    }
    else {
        yRange = destHeight;
        yStart = 0;
    }

    int xRange, xStart;
    if (xDest < 0) {
        xRange = xDest + destWidth;
        xStart = -xDest;
        xDest = 0;
    }
    else {
        xRange = destWidth;
        xStart = 0;
    }

    uint32_t size = xRange * yRange;
    color* buffer = new color[size];
    uint32_t i = 0;

    if (srcWidth == destWidth && srcHeight == destHeight)
        for (int y = 0; y < yRange; ++y)
            for (int x = 0; x < xRange; ++x)
                buffer[i++] = _color(imSrc, x + xSrc + xStart, y + ySrc + yStart);
    else
        for (int y = 0; y < yRange; ++y)
            for (int x = 0; x < xRange; ++x)
                buffer[i++] = _color(imSrc,
                    x*srcWidth / destWidth + xSrc + xStart,
                    y*srcHeight / destHeight + ySrc + yStart);

    cairo_format_t format = CAIRO_FORMAT_ARGB32;
    int stride = cairo_format_stride_for_width(format, xRange);
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        (uint8_t*)buffer, // a pointer to a buffer supplied by the application
        format, // the format of pixels in the buffer
        xRange, // the width of the image to be stored in the buffer
        yRange, // the height of the image to be stored in the buffer
        stride  // the number of bytes between the start of rows in the buffer as allocated.
    );
    cairo_set_source_surface(cr, surface, xDest, yDest);
    
    cairo_rectangle(cr, xDest, yDest, xRange, yRange);
    cairo_fill(cr);
    cairo_stroke(cr); 

    cairo_surface_finish(surface);
    delete[] buffer;
}

#endif /* _GTK */
