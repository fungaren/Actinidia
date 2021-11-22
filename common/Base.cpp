/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#ifdef _WIN32
#include <Windows.h>
#elif defined _GTK
#else
#error unsupported platform
#endif

#include <stdexcept>

#include "Base.h"

#ifdef _WIN32

std::wstring fromUTF8(const std::string& s) noexcept(false) {
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    std::wstring ws;
    ws.resize(n - 1); // Deduct terminated zero
    if (MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws[0], n) == 0) {
        (void)GetLastError();
        throw ustr_error(ustr("Failed to load UTF-8"));
    }
    return ws;
}

std::string toUTF8(const std::wstring& ws) noexcept(false) {
    int n = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
    std::string s;
    s.resize(n - 1); // Deduct terminated zero
    if (WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &s[0], n, NULL, NULL) == 0) {
        (void)GetLastError();
        throw ustr_error(ustr("Failed to convert to UTF-8"));
    }
    return s;
}

#elif defined _GTK
#else
#error unsupported platform
#endif
