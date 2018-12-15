#pragma once
#include "Canvas.h"

class Window
{
	std::thread tWnd;
	std::wstring title;
	uint16_t width;
	uint16_t height;

	void looper() const;

	std::function<void(int)> onKeyDown = [](int) {};
	std::function<void(int)> onKeyUp = [](int) {};
	std::function<void(uint32_t, int, int)> onLButtonDown = [](uint32_t, int, int) {};
	std::function<void(uint32_t, int, int)> onLButtonUp = [](uint32_t, int, int) {};
	std::function<void(uint32_t, int, int)> onRButtonDown = [](uint32_t, int, int) {};
	std::function<void(uint32_t, int, int)> onRButtonUp = [](uint32_t, int, int) {};
	std::function<void(uint32_t, int, int)> onMouseMove = [](uint32_t, int, int) {};
	std::function<void(short, int, int)> onMouseWheel = [](short, int, int) {};
	std::function<void(const GdiCanvas&)> onPaint = [](const GdiCanvas&) {};

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
	void create(const std::wstring& title, uint16_t width = 600, uint16_t height = 400) {
		if (title.empty())
			return;
		this->title = title;
		this->width = width;
		this->height = height;
		tWnd = std::thread(&Window::looper, this);
	}

	template <typename T>
	void setKeyDownHandler(T& handler)		{ isRunning(); onKeyDown = handler; }
	template <typename T>
	void setKeyUpHandler(T& handler)		{ isRunning(); onKeyUp = handler; }
	template <typename T>
	void setLButtonDownHandler(T& handler)	{ isRunning(); onLButtonDown = handler; }
	template <typename T>
	void setLButtonUpHandler(T& handler)	{ isRunning(); onLButtonUp = handler; }
	template <typename T>
	void setRButtonDownHandler(T& handler)	{ isRunning(); onRButtonDown = handler; }
	template <typename T>
	void setRButtonUpHandler(T& handler)	{ isRunning(); onRButtonUp = handler; }
	template <typename T>
	void setMouseMoveHandler(T& handler)	{ isRunning(); onMouseMove = handler; }
	template <typename T>
	void setMouseWheelHandler(T& handler)	{ isRunning(); onMouseWheel = handler; }
	template <typename T>
	void setPaintHandler(T& handler)		{ isRunning(); onPaint = handler; }

	auto& getKeyDownHandler() const			{ return onKeyDown; }
	auto& getKeyUpHandler() const			{ return onKeyUp; }
	auto& getLButtonDownHandler() const		{ return onLButtonDown; }
	auto& getLButtonUpHandler() const		{ return onLButtonUp; }
	auto& getRButtonDownHandler() const		{ return onRButtonDown; }
	auto& getRButtonUpHandler() const		{ return onRButtonUp; }
	auto& getMouseMoveHandler() const		{ return onMouseMove; }
	auto& getMouseWheelHandler() const		{ return onMouseWheel; }
	auto& getPaintHandler() const			{ return onPaint; }

};
