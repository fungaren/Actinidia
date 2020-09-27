/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#pragma once
#ifdef _WIN32
    #include <windows.h>
    #undef max
    #include "../libpng/png.h"
    #include "../zlib/zlib.h"
    extern "C" {
    #include "../libjpeg/jpeglib.h"
    }
#endif /* _WIN32 */
#ifdef _GTK
    #include "png.h"
    #include "zlib.h"
    #include "jpeglib.h"
#endif /* _GTK */
#include <cstdint>
#include <cstring>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <memory>

class Matrix {
protected:
    Matrix() = default;
    Matrix(Matrix&) = delete;
    Matrix(Matrix&&) = default;
public:
    virtual uint16_t getWidth() const = 0;
    virtual uint16_t getHeight() const = 0;
    virtual uint32_t* getMatrix() const = 0;
};

class ImageMatrix : Matrix {
    friend class ImageMatrixFactory;
    
    uint32_t* matrix;
    uint16_t width;
    uint16_t height;
    
    ImageMatrix(uint32_t *table, uint16_t width, uint16_t height)
        : matrix(table), width(width), height(height)
    {}

public:
    ImageMatrix()
        : matrix(nullptr), width(0), height(0) {}
    ImageMatrix(ImageMatrix&) = delete;
    ImageMatrix(ImageMatrix&& src) noexcept
        : matrix(src.matrix), width(src.width), height(src.height) {
        src.matrix = nullptr;
    }
    void operator=(ImageMatrix&& src) noexcept {
        this->~ImageMatrix();
        width = src.width;
        height = src.height;
        matrix = src.matrix;
        src.matrix = nullptr;
    }

    void discardAlphaChannel() noexcept {
        if (matrix == nullptr)
            return;
        for (uint32_t i = 0; i < (uint32_t)width*height; ++i)
            matrix[i] |= 0xFF000000;
    }

    uint16_t getWidth() const { return width; };
    uint16_t getHeight() const { return height; };
    uint32_t* getMatrix() const { return matrix; };

    ~ImageMatrix() {
        if (matrix == nullptr)
            return;
        delete[] matrix;
    }

    #define _pos(im, x, y) ((y)*(im->getWidth())+(x))
    #define _color(im, x, y) im->getMatrix()[_pos(im, x, y)]
};

typedef std::shared_ptr<ImageMatrix> pImageMatrix;

class ImageMatrixFactory {

    static void png_read_data_fn(png_structp png_ptr, png_bytep dest, png_size_t length);

    struct _jpeg_error_mgr
    {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
        void(*emit_message)(j_common_ptr, int);
        boolean warning;
    };

    static void jpeg_error_exit(j_common_ptr cinfo);

    static pImageMatrix fromPngFile(FILE *fp);
    static pImageMatrix readPngImpl(png_structp png_ptr, png_infop info_ptr);
    static pImageMatrix fromJpegFile(FILE *fp);
    static pImageMatrix readJpegImpl(jpeg_decompress_struct& cinfo, _jpeg_error_mgr& jerr);

    static void dumpJpegFile(const pImageMatrix im, FILE *fp, uint8_t quality);
    static void dumpPngFile(const pImageMatrix im, FILE *fp);

    /**
     * @param str A wide-char string.
     * @return A UTF-8 encoded C-type string.
     */
    static const char* utf8(const std::wstring& str) {
        static std::string s; // for temporary use
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
        s = cvt.to_bytes(str);
        return s.c_str();
    }
    
    static FILE* openfile(const char* path, const char* mode) noexcept(false) {
        FILE *fp;
#ifdef _WIN32
        char buffer[240];
        if (fopen_s(&fp, path, mode))
        {
            strerror_s(buffer, sizeof buffer, errno);
            throw std::runtime_error(
                std::string("Failed to open file: ") + buffer
            );
        }
#endif /* _WIN32 */
#ifdef _GTK
        fp = fopen(path, mode);
        if (fp == NULL)
            throw std::runtime_error(
                std::string("Failed to open file: ") + strerror(errno)
            );
#endif /* _GTK */
        return fp;
    }
    static FILE* openfile(const wchar_t* path, const wchar_t* mode) noexcept(false) {
        FILE *fp;
#ifdef _WIN32
        char buffer[240];
        if (_wfopen_s(&fp, path, mode))
        {
            strerror_s(buffer, sizeof buffer, errno);
            throw std::runtime_error(
                std::string("Failed to open file: ") + buffer
            );
        }
#endif /* _WIN32 */
#ifdef _GTK
        fp = fopen(utf8(path), utf8(mode));
        if (fp == NULL)
            throw std::runtime_error(
                std::string("Failed to open file: ") + strerror(errno)
            );
#endif /* _GTK */
        return fp;
    }
public:
    ImageMatrixFactory() = delete;

    struct mem_image {
        size_t size;
        size_t has_read = 8;
        void* addr;
        mem_image(void* pImageRes, size_t len)
            : size(len), addr(pImageRes) {}
    };

    static pImageMatrix fromPixelData(uint32_t* data, uint16_t width, uint16_t height) {
        return std::shared_ptr<ImageMatrix>(new ImageMatrix(data, width, height));
    }

    /**
     * @param width Width of buffer image.
     * @param height Height of the buffer image.
     * @param bkgrdColor Background color of the buffer image.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix createBufferImage(uint16_t width, uint16_t height,
        uint32_t bkgrdColor = 0xFFFFFFFF) noexcept(false);

    /**
     * @param pngFile The file path of the png file.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromPngFile(const char* pngFile) noexcept(false);

    /**
     * @param pngFile The file path of the png file.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromPngFile(const wchar_t* pngFile) noexcept(false);

    /**
     * @param mp A mem_image structure pointer.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromPngBuffer(mem_image *mp) noexcept(false);

    /**
     * @param pImageRes The png file buffer in memory.
     * @param len The buffer length of the png file in memory.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromPngBuffer(void* pImageRes, size_t len) noexcept(false) {
        mem_image img(pImageRes, len);
        return fromPngBuffer(&img);
    }

#ifdef _WIN32
    /**
     * @param nResID The resource id.
     * @param lpType The resource type in wchar string.
     * @param hModule The module handle of the resource.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromPngResource(UINT nResID, LPCTSTR lpType, HMODULE hModule) noexcept(false);
#endif /* _WIN32 */
#ifdef _GTK

#endif /* _GTK */

    /**
     * @param jpegFile The file path of the jpeg file.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromJpegFile(const char* jpegFile) noexcept(false);

    /**
     * @param jpegFile The file path of the jpeg file.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromJpegFile(const wchar_t* jpegFile) noexcept(false);

    /**
     * @param mp A mem_image structure pointer.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromJpegBuffer(mem_image *mp) noexcept(false);

    /**
     * @param pImageRes The png file buffer in memory.
     * @param len The buffer length of the png file in memory.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromJpegBuffer(void* pImageRes, size_t len) noexcept(false) {
        mem_image img(pImageRes, len);
        return fromJpegBuffer(&img);
    }

#ifdef _WIN32
    /**
     * @param nResID The resource id.
     * @param lpType The resource type in wchar string.
     * @param hModule The module handle of the resource.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromJpegResource(UINT nResID, LPCTSTR lpType, HMODULE hModule) noexcept(false);
#endif /* _WIN32 */
#ifdef _GTK

#endif /* _GTK */
    
    /**
     * @param imageFile The file path of the image file. Only png and jpeg 
     *                  format are supported.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix openImage(const char* imageFile) noexcept(false)
    {
        std::string extname = imageFile + strlen(imageFile) - 3;
        if (extname == "png" || extname == "PNG")
            return ImageMatrixFactory::fromPngFile(imageFile);
        else if (extname == "jpg" || extname == "JPG")
            return ImageMatrixFactory::fromJpegFile(imageFile);
        else 
            throw std::runtime_error(
                std::string("Unknown image file: ") + imageFile
            );
    }

    /**
     * @param im pImageMatix handle
     * @param filePath the save path of the jpeg file.
     * @param quality range [1,100]
     */
    static void dumpJpegFile(const pImageMatrix im, 
        const char* filePath, uint8_t quality = 80) noexcept(false);

    /**
     * @param im pImageMatix handle
     * @param filePath the save path of the jpeg file.
     * @param quality range [1,100]
     */
    static void dumpJpegFile(const pImageMatrix im, 
        const wchar_t* filePath, uint8_t quality = 80) noexcept(false);

    /**
     * @param im pImageMatix handle
     * @param filePath the save path of the png file.
     */
    static void dumpPngFile(const pImageMatrix im, 
        const char* filePath) noexcept(false);

    /**
     * @param im pImageMatix handle
     * @param filePath the save path of the png file.
     */
    static void dumpPngFile(const pImageMatrix im, 
        const wchar_t* filePath) noexcept(false);
};
