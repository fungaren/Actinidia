/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#pragma once

/** 
 * @param code The error code.
 * @return Corresponding text to the error code.
 */
std::string zerr(int code);
/**
 * @param code The error code.
 * @return Corresponding text to the error code.
 */
std::wstring zerrw(int code);
/**
 * @param source The input file path.
 * @param dest The output file path.
 * @param level The compress level, from 0 to 9. Greater level takes more time.
 * @return 0 for success, otherwise return the error number.
 */
int compress(std::istream& source, std::ostream& dest, int level);
/**
 * @param source The input file path.
 * @param dest The output file path.
 * @return 0 for success, otherwise return the error number.
 */
int decompress(std::istream& source, std::ostream& dest);
