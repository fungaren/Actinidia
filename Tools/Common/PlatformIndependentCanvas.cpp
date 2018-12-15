#include "pch.h"
#include "ImageMatrix.h"
#include "Canvas.h"

Canvas::color PlatformIndependenceCanvas::getPixel(const ImageMatrix& im, int x, int y) noexcept(false)
{
	if (x < 0 || y < 0 || x >= im.getWidth() || y >= im.getHeight())
		throw (std::invalid_argument("Coordinate out of range"));
	else
		return (color)im.getMatrix()[y][x];
}

void PlatformIndependenceCanvas::fillSolidRect(ImageMatrix& im, int left, int top, int right, int bottom, color fillColor)
{
	if (im.getMatrix() == nullptr)
		throw std::invalid_argument("ImageMatrix is not initialized");

	if (left < 0) left = 0;
	if (top < 0) top = 0;
	if (right >= im.getWidth()) right = im.getWidth() - 1;
	if (bottom >= im.getHeight()) bottom = im.getHeight() - 1;

	for (int y = top; y < bottom; ++y) {
		for (int x = left; x < right; ++x) {
			im.getMatrix()[y][x] = fillColor;
		}
	}
}

void PlatformIndependenceCanvas::drawLine(ImageMatrix& im, int x1, int y1, int x2, int y2, LineStyle ls, color lineColor)
{
	if (im.getMatrix() == nullptr)
		throw std::invalid_argument("ImageMatrix is not initialized");

	uint16_t h = im.getHeight();
	uint16_t w = im.getWidth();
	// coordinates may not in rectangle
	if (x1 == x2) {
		if (x1 < 0 || x1 >= w) return;
		if (y1 < 0) y1 = 0;
		if (y1 >= h) y1 = h - 1;
		if (y2 < 0) y2 = 0;
		if (y2 >= h) y2 = h - 1;
		if (y1 < y2)
			for (uint16_t y = y1; y <= y2; ++y)
				im.getMatrix()[y][x1] = lineColor;
		else
			for (uint16_t y = y2; y <= y1; ++y)
				im.getMatrix()[y][x1] = lineColor;
	}
	else if (y1 == y2) {
		if (y1 < 0 || y1 >= h) return;
		if (x1 < 0) x1 = 0;
		if (x1 >= w) x1 = w - 1;
		if (x2 < 0) x2 = 0;
		if (x2 >= w) x2 = w - 1;
		if (x1 < x2)
			for (uint16_t x = x1; x <= x2; ++x)
				im.getMatrix()[y1][x] = lineColor;
		else
			for (uint16_t x = x2; x <= x1; ++x)
				im.getMatrix()[y1][x] = lineColor;
	}
	else {
		double k = (y2 - y1) / (double)(x2 - x1);
		double b = y2 - x2 * k;
		bool p1_outside = x1 < 0 || x1 >= w || y1 < 0 || y1 >= h;
		bool p2_outside = x2 < 0 || x2 >= w || y2 < 0 || y2 >= h;
		if (!p1_outside && !p2_outside) {	// two coordinates are both inside the rectangle
			;
		}
		else {	// Either one or two coordinates are outside the rectangle
			// y = k*x+b
			int xTop = (int)(-b / k);				// Intersection point on top edge (y=0)
			int xBottom = (int)((h - 1 - b) / k);	// Intersection point on bottom edge (y=h-1)
			int yLeft = (int)b;						// Intersection point on left edge (x=0)
			int yRight = (int)(k * (w - 1) + b);	// Intersection point on right edge (x=w-1)

			// two coordinates are both outside the rectangle
			if (p1_outside && p2_outside) {
				bool on_top = (xTop >= 0 && xTop < w);
				bool on_bottom = (xBottom >= 0 && xBottom < w);
				bool on_left = (yLeft >= 0 && yLeft < h);
				bool on_right = (yRight >= 0 && yRight < h);
				// so there're two intersection points
				if (on_top) {
					x1 = xTop; y1 = 0;
					if (on_bottom) {
						x2 = xBottom; y2 = h - 1;
					}
					else if (on_left) {
						x2 = 0; y2 = yLeft;
					}
					else /*if (on_right)*/ {
						x2 = w - 1; y2 = yRight;
					}
				}
				else if (on_bottom) {
					x1 = xBottom; y1 = h - 1;
					if (on_left) {
						x2 = 0; y2 = yLeft;
					}
					else /*if (on_right)*/ {
						x2 = w - 1; y2 = yRight;
					}
				}
				else if (on_left) {
					x1 = 0; y1 = yLeft;
					/*if (on_right)*/
					x2 = w - 1; y2 = yRight;
				}
				else
					return;
			}
			else if (p1_outside) {
				// use the only intersection point between p1 and p2
				if (x1 < x2) {
					if (xTop >= 0 && xTop <= x2) {
						x1 = xTop; y1 = 0;
					}
					if (xBottom >= 0 && xBottom < x2) {
						x1 = xBottom; y1 = h - 1;
					}
					if (yLeft >= 0 && yLeft < h) {
						x1 = 0; y1 = yLeft;
					}
				}
				else { // x1 > x2
					if (xTop < w && xTop >= x2) {
						x1 = xTop; y1 = 0;
					}
					if (xBottom < w && xBottom > x2) {
						x1 = xBottom; y1 = h - 1;
					}
					if (yRight >= 0 && yRight < h) {
						x1 = w - 1; y1 = yRight;
					}
				}
			}
			else { // p2_outside
				if (x2 > x1) {
					if (xTop >= x1 && xTop < w) {
						x2 = xTop; y2 = 0;
					}
					if (xBottom < w && xBottom > x1) {
						x2 = xBottom; y2 = h - 1;
					}
					if (yRight >= 0 && yRight < h) {
						x2 = w - 1; y2 = yRight;
					}
				}
				else { // x2 < x1
					if (xTop >= 0 && xTop <= x1) {
						x2 = xTop; y2 = 0;
					}
					if (xBottom >= 0 && xBottom < x1) {
						x2 = xBottom; y2 = h - 1;
					}
					if (yLeft >= 0 && yLeft < h) {
						x2 = 0; y2 = yLeft;
					}
				}
			}
		}
		if (-1 <= k && k <= 1) {
			if (x1 < x2)
				for (uint16_t x = x1; x <= x2; ++x)
					im.getMatrix()[(int)(k*x + b)][x] = lineColor;
			else
				for (uint16_t x = x2; x <= x1; ++x)
					im.getMatrix()[(int)(k*x + b)][x] = lineColor;
		} else {
			k = 1 / k;
			if (y1 < y2)
				for (uint16_t y = y1; y <= y2; ++y)
					im.getMatrix()[y][(int)((y - b)*k)] = lineColor;
			else
				for (uint16_t y = y2; y <= y1; ++y)
					im.getMatrix()[y][(int)((y - b)*k)] = lineColor;
		}
	}
}

void PlatformIndependenceCanvas::rectangle(ImageMatrix& im, int left, int top, int right, int bottom, LineStyle ls, color lineColor)
{
	drawLine(im, left, top, right, top, ls, lineColor);
	drawLine(im, left, top, left, bottom, ls, lineColor);
	drawLine(im, right, top, right, bottom, ls, lineColor);
	drawLine(im, left, bottom, right, bottom, ls, lineColor);
}

void PlatformIndependenceCanvas::blend(ImageMatrix& imDest, const ImageMatrix& imSrc,
	int xDest, int yDest, uint8_t opacity)
{
	if (imDest.getMatrix() == nullptr || imSrc.getMatrix() == nullptr)
		throw std::invalid_argument("ImageMatrix is not initialized");
	if (imDest.getMatrix() == imSrc.getMatrix())
		throw std::invalid_argument("Source and destination cannot be the same");

	int yRange, yStart;
	if (yDest < 0) {
		yRange = yDest + imSrc.getHeight() > imDest.getHeight() ? imDest.getHeight() : yDest + imSrc.getHeight();
		yStart = -yDest;
		yDest = 0;
	}
	else {
		yRange = yDest + imSrc.getHeight() > imDest.getHeight() ? imDest.getHeight() - yDest : imSrc.getHeight();
		yStart = 0;
	}

	int xRange, xStart;
	if (xDest < 0) {
		xRange = xDest + imSrc.getWidth() > imDest.getWidth() ? imDest.getWidth() : xDest + imSrc.getWidth();
		xStart = -xDest;
		xDest = 0;
	}
	else {
		xRange = xDest + imSrc.getWidth() > imDest.getWidth() ? imDest.getWidth() - xDest : imSrc.getWidth();
		xStart = 0;
	}

	for (int y = 0; y < yRange; y++) {
		for (int x = 0; x < xRange; x++) {
			color src = imSrc.getMatrix()[y + yStart][x + xStart];
			color *dst = &(imDest.getMatrix()[yDest + y][xDest + x]);
			*dst = Canvas::blend(*dst, src, opacity);
		}
	}
}

void PlatformIndependenceCanvas::blend(ImageMatrix& imDest, const ImageMatrix& imSrc,
	int xDest, int yDest, int destWidth, int destHeight,
	int xSrc, int ySrc, int srcWidth, int srcHeight, uint8_t opacity)
{
	if (imDest.getMatrix() == nullptr || imSrc.getMatrix() == nullptr)
		throw std::invalid_argument("ImageMatrix is not initialized");

	if (ySrc + srcHeight > imSrc.getHeight() || xSrc + srcWidth > imSrc.getWidth())
		return;

	int yRange, yStart;
	if (yDest < 0) {
		yRange = yDest + destHeight > imDest.getHeight() ? imDest.getHeight() : yDest + destHeight;
		yStart = -yDest;
		yDest = 0;
	}
	else {
		yRange = yDest + destHeight > imDest.getHeight() ? imDest.getHeight() - yDest : destHeight;
		yStart = 0;
	}

	int xRange, xStart;
	if (xDest < 0) {
		xRange = xDest + destWidth > imDest.getWidth() ? imDest.getWidth() : xDest + destWidth;
		xStart = -xDest;
		xDest = 0;
	}
	else {
		xRange = xDest + destWidth > imDest.getWidth() ? imDest.getWidth() - xDest : destWidth;
		xStart = 0;
	}

	if (srcWidth == destWidth && srcHeight == destHeight)
		for (int y = 0; y < yRange ; y++) {
			for (int x = 0; x < xRange; x++) {
				color src = imSrc.getMatrix()[y + ySrc + yStart][x + xSrc + xStart];
				color *dst = &(imDest.getMatrix()[yDest + y][xDest + x]);
				*dst = Canvas::blend(*dst, src, opacity);
			}
		}
	else
		for (int y = 0; y < yRange; y++) {
			for (int x = 0; x < xRange; x++) {
				color src = imSrc.getMatrix()
					[y*srcHeight / destHeight + ySrc + yStart][x*srcWidth / destWidth + xSrc + xStart];
				color *dst = &(imDest.getMatrix()[yDest + y][xDest + x]);
				*dst = Canvas::blend(*dst, src, opacity);
			}
		}
}
