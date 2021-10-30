/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#pragma once
#include <string>
#include <sstream>

#ifdef _WIN32

#define ustr(x) L ## x
typedef wchar_t char_t;
typedef std::wstring string_t;
typedef std::wstringstream stringstream_t;

// path::wstring()
#define to_ustr wstring

std::wstring fromUTF8(const std::string& s) noexcept(false);
std::string toUTF8(const std::wstring& s) noexcept(false);

#elif defined _GTK

#define ustr(x) x
/* Should be char8_t, just for compatibility */
typedef char char_t;
/* Should be std::u8string, just for compatibility */
typedef std::string string_t;
typedef std::stringstream stringstream_t;

// path::string()
#define to_ustr string

inline std::string fromUTF8(const std::string& s) { return s; };
inline std::string toUTF8(const std::string& s) { return s; };

#else
#error unsupported platform
#endif

class ustr_error {
public:
    ustr_error(const string_t& what) :data(what) {}
    virtual const string_t& what() noexcept {
        return data;
    }
private:
    string_t data;
};
