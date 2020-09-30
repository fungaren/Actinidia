/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
    #include <windows.h>
    #undef max
    #include "../zlib/zlib.h"
#endif /* _WIN32 */
#ifdef _GTK
    #include "zlib.h"
#endif /* _GTK */
#include <string>
#include <fstream>
#include <filesystem>
#include "Compress.h"

#ifndef _ASSERT
    #define _ASSERT(...)
#endif /* _ASSERT */

std::string zerr(int code)
{
	switch (code)
	{
	case Z_ERRNO:
		return "error reading/writing stream";
	case Z_STREAM_ERROR:
		return "invalid compression level";
	case Z_DATA_ERROR:
		return "invalid or incomplete deflate data\n";
	case Z_MEM_ERROR:
		return "out of memory";
	case Z_VERSION_ERROR:
		return "zlib version of the library linked mismatch!";
	}
	return "unknown error";
}

std::wstring zerrw(int code)
{
    switch (code)
    {
    case Z_ERRNO:
        return L"error reading/writing stream";
    case Z_STREAM_ERROR:
        return L"invalid compression level";
    case Z_DATA_ERROR:
        return L"invalid or incomplete deflate data\n";
    case Z_MEM_ERROR:
        return L"out of memory";
    case Z_VERSION_ERROR:
        return L"zlib version of the library linked mismatch!";
    }
    return L"unknown error";
}

#define CHUNK 4096
int compress(std::istream& source, std::ostream& dest, int level)
{
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];

    // allocate deflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    int flush;
    // compress until end of file
    do {
        source.read(in, CHUNK);
        strm.avail_in = (uInt)source.gcount();
        if (source.bad()) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = source.eof() ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = (unsigned char*)in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (unsigned char*)out;
            ret = deflate(&strm, flush);    /* no bad return value */
            _ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
            unsigned have = CHUNK - strm.avail_out;
            dest.write(out, have);
            if (dest.bad()) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        _ASSERT(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    _ASSERT(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}

int decompress(std::istream& source, std::ostream& dest)
{
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];

    // allocate inflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    int ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        source.read(in, CHUNK);
        strm.avail_in = (uInt)source.gcount();
        if (source.bad()) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = (unsigned char*)in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (unsigned char*)out;
            ret = inflate(&strm, Z_NO_FLUSH);
            _ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            unsigned have = CHUNK - strm.avail_out;
            dest.write(out, have);
            if (dest.bad()) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
