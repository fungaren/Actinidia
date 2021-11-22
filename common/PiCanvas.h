/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#pragma once
#include "Canvas.h"
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
    static void printText(pImageMatrix im, int x, int y, const string_t& str, uint16_t len,
        const string_t& fontName, uint8_t fontSize, color fontColor = Constant::black,
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
