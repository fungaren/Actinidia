/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#pragma once
#include <fstream>

#include "Base.h"

/**
 * @param code The error code.
 * @return Corresponding text to the error code.
 */
string_t zerr(int code);

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
