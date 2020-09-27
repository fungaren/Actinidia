/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
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
