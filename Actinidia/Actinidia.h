/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#pragma once
#include "../Tools/Common/Window.h"

bool LuaInit();
void OnClean();
void OnSetFocus();
void OnKillFocus();
void OnPaint(const GdiCanvas& gdi);
void OnClose();
void OnKeyDown(int key);
void OnKeyUp(int key);
void OnLButtonDown(uint32_t, int x, int y);
void OnLButtonUp(uint32_t, int x, int y);
void OnMouseMove(uint32_t, int x, int y);
void OnMouseWheel(uint32_t, short zDelta, int x, int y);

#define MIN_WIDTH 200
#define MIN_HEIGHT 100
