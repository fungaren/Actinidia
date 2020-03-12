#pragma once
std::string zerr(int code);
std::wstring zerrw(int code);
// level is from 0 to 9
int compress(std::istream& source, std::ostream& dest, int level);
int decompress(std::istream& source, std::ostream& dest);
