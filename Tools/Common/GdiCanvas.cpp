#ifdef _WIN32
    #include "pch.h"
#endif /* _WIN32 */
#ifdef _GTK

#endif /* _GTK */
#include <iostream>
#include <string>
#include <sstream>
#include <future>
#include <chrono>
#include <fstream>
#include <experimental/filesystem>
#include <thread>
#include <map>
#ifdef _WIN32
#include "ImageMatrix.h"
#include "Canvas.h"

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
            buffer[i++] = imSrc->getMatrix()[y + yStart][x + xStart];

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

    SetDIBitsToDevice(hdc, xDest, yDest, xRange, yRange,
        0, 0, 0, yRange, buffer, &bmpi, DIB_RGB_COLORS);

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
                buffer[i++] = imSrc->getMatrix()[y + ySrc + yStart][x + xSrc + xStart];
    else
        for (int y = 0; y < yRange; ++y)
            for (int x = 0; x < xRange; ++x)
                buffer[i++] = imSrc->getMatrix()
                [y*srcHeight / destHeight + ySrc + yStart][x*srcWidth / destWidth + xSrc + xStart];

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

    SetDIBitsToDevice(hdc, xDest, yDest, xRange, yRange,
        0, 0, 0, yRange, buffer, &bmpi, DIB_RGB_COLORS);

    delete[] buffer;
}

#endif /* _WIN32 */
#ifdef _GTK

#endif /* _GTK */
