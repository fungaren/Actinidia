#pragma once
#include "ImageMatrix.h"

class Canvas
{
public:

	typedef uint8_t CharStyle;
	
	// 0xAARRGGBB (A: opacity, R: red, G: green, B: blue)
	typedef uint32_t color;
	struct Constant {
		static const color alpha	= 0x00FFFFFF;
		static const color white	= 0xFFFFFFFF;
		static const color black	= 0xFF000000;
		static const color red		= 0xFFFF0000;
		static const color green	= 0xFF00FF00;
		static const color blue		= 0xFF0000FF;
		static const color cyan		= 0xFF00FFFF;
		static const color magenta	= 0xFFFF00FF;
		static const color yellow	= 0xFFFFFF00;

		static const CharStyle style_default	= { 0 };
		static const CharStyle style_bold		= { 1 << 7 };
		static const CharStyle style_itatic		= { 1 << 6 };
		static const CharStyle style_underline	= { 1 << 5 };
	};

	static color toABGR(color argb) {
		uint8_t b = (uint8_t)(argb >> 8 * 0);
		uint8_t r = (uint8_t)(argb >> 8 * 2);
		argb &= 0xFF00FF00;
		return argb | (b << 8 * 2) | r;
	}

	enum LineStyle {
		solid	= PS_SOLID,
		dashed	= PS_DASH,
		dotted	= PS_DOT,
		none	= PS_NULL
	};

	/// <summary>
	/// @param int opacity: 0-255
	/// </summary>
	static color blend(color dest, color src, uint8_t opacity) {
		// Alpha
		uint32_t channel_src = src >> 8 * 3;
		uint32_t channel_dst = dest >> 8 * 3;
		opacity = channel_src * opacity / 256;	// divide by 256 for better performance
		color mixed = channel_src + channel_dst;
		mixed = mixed > 255 ? 0xFF00 : mixed << 8;
		// Red
		channel_src = (src >> 8 * 2) & 0xFF;
		channel_dst = (dest >> 8 * 2) & 0xFF;
		mixed |= (channel_src * opacity + channel_dst * (255 - opacity)) / 256;
		mixed <<= 8;
		// Green
		channel_src = (src >> 8 * 1) & 0xFF;
		channel_dst = (dest >> 8 * 1) & 0xFF;
		mixed |= (channel_src * opacity + channel_dst * (255 - opacity)) / 256;
		mixed <<= 8;
		// Blue
		channel_src = (src >> 8 * 0) & 0xFF;
		channel_dst = (dest >> 8 * 0) & 0xFF;
		mixed |= (channel_src * opacity + channel_dst * (255 - opacity)) / 256;
		return mixed;
	}
};

class PlatformIndependenceCanvas : Canvas
{
	PlatformIndependenceCanvas();
public:
	static color getPixel(const ImageMatrix& im, int x, int y) noexcept(false);

	static void fillSolidRect(ImageMatrix& im, int left, int top, int right, int bottom, color fillColor);

	static bool printText(ImageMatrix& im, int x, int y, std::wstring str, uint16_t len,
		std::wstring fontName, uint8_t fontSize, color fontColor = Constant::black,
		CharStyle style = Constant::style_default) {
		return false;
	}

	static void drawLine(ImageMatrix& im, int x1, int y1, int x2, int y2,
		LineStyle ls = LineStyle::solid, color lineColor = Constant::black);

	static void rectangle(ImageMatrix& im, int left, int top, int right, int bottom,
		LineStyle ls = LineStyle::solid, color lineColor = Constant::black);

	static void blend(ImageMatrix& imDest, const ImageMatrix& imSrc,
		int xDest, int yDest, uint8_t opacity);

	static void blend(ImageMatrix& imDest, const ImageMatrix& imSrc,
		int xDest, int yDest, int destWidth, int destHeight,
		int xSrc, int ySrc, int srcWidth, int srcHeight, uint8_t opacity);
};

typedef PlatformIndependenceCanvas PiCanvas;

#ifdef _WIN32
class GdiCanvas : Canvas
{
private:
	HWND hwnd;
	HDC hdc;

public:

	GdiCanvas(HWND hWnd)
		:hwnd(hWnd),
		hdc(GetDC(hWnd))
	{ }

	GdiCanvas(HWND hWnd, HDC hdc)
		:hwnd(hWnd),
		hdc(hdc)
	{ }

	GdiCanvas(GdiCanvas&) = delete;

	~GdiCanvas() {
		ReleaseDC(hwnd, hdc);
	}

	void fillSolidRect(int left, int top, int right, int bottom, color fillColor) const;

	//const unsigned int format = DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_NOCLIP | DT_HIDEPREFIX;
	//// calc expected height for specific text width
	//int calcHeight(const std::wstring& str, int width)
	//{
	//	RECT rc = { 0,0,width,0 };
	//	DrawTextW(hdc, str.c_str(), str.length(), &rc, format | DT_CALCRECT);
	//	return rc.bottom - rc.top;
	//}

	//Font& drawText(const std::wstring& str, int left, int top, int width, int height) {
	//	RECT rc = { left, top, left + width, top + height };

	//	SetBkMode(hdc, TRANSPARENT);
	//	SetTextColor(hdc, color);

	//	DrawTextW(hdc, str.c_str(), str.length(), &rc, format);
	//	return *this;
	//}

	bool printText(int x, int y, std::wstring str, uint16_t len,
		std::wstring fontName, uint8_t fontSize, color fontColor = Constant::black, 
		CharStyle style = Constant::style_default) const;

	void drawLine(int x1, int y1, int x2, int y2,
		LineStyle ls = LineStyle::solid, color lineColor = Constant::black) const;

	void rectangle(int left, int top, int right, int bottom,
		LineStyle ls = LineStyle::solid, color lineColor = Constant::black) const;

	///<summary>
	///	Do not support alpha blend. The alpha channel will be discard.
	///</summary>
	void paste(const ImageMatrix& imSrc, int xDest, int yDest) const;

	///<summary>
	///	Do not support alpha blend. The alpha channel will be discard.
	///</summary>
	void paste(const ImageMatrix& imSrc,
		int xDest, int yDest, int destWidth, int destHeight,
		int xSrc, int ySrc, int srcWidth, int srcHeight) const;
};
#endif
