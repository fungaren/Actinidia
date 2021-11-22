/*
 * Copyright (c) 2021, FANG All rights reserved.
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
#elif defined _GTK
    #include <png.h>
    #include <zlib.h>
    #include <jpeglib.h>
    #include <cstring>
#else
#error unsupported platform
#endif
#include <stdexcept>
#include <memory>

#include "Base.h"

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

    static FILE* openfile(const string_t& path, const char_t *mode) noexcept(false) {
        FILE *fp;
#ifdef _WIN32
        char buffer[240];
        if (_wfopen_s(&fp, path.c_str(), mode))
        {
            strerror_s(buffer, sizeof buffer, errno);
            throw ustr_error(fromUTF8(std::string("Failed to open file: ") + buffer));
        }
#elif defined _GTK
        fp = fopen(path.c_str(), mode);
        if (fp == NULL)
            throw ustr_error(string_t("Failed to open file: ") + strerror(errno));
#else
#error unsupported platform
#endif
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

    /**
     * @param data Pixel data buffer. The memory will be freed automatically when
     *             the ImageMatrix is destructed.
     * @param width width of pixel data.
     * @param height height of pixel data.
     */
    static pImageMatrix fromPixelData(
        uint32_t* data, uint16_t width, uint16_t height) noexcept(false)
    {
        if (data == nullptr)
            throw ustr_error(ustr("Buffer pointer is null"));
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
    static pImageMatrix fromPngFile(const string_t& pngFile) noexcept(false);

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
#elif defined _GTK
#else
#error unsupported platform
#endif

    /**
     * @param jpegFile The file path of the jpeg file.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromJpegFile(const string_t& jpegFile) noexcept(false);

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
#elif defined _GTK
#else
#error unsupported platform
#endif

    /**
     * @param imageFile The file path of the image file. Only png and jpeg
     *                  format are supported.
     * @return A pImageMatrix handle.
     */
    static pImageMatrix fromImageFile(const string_t& imageFile) noexcept(false)
    {
        string_t extname = imageFile.substr(imageFile.size() - 3);
        if (extname == ustr("png") || extname == ustr("PNG"))
            return ImageMatrixFactory::fromPngFile(imageFile);
        else if (extname == ustr("jpg") || extname == ustr("JPG"))
            return ImageMatrixFactory::fromJpegFile(imageFile);
        else
            throw ustr_error(ustr("Unknown image file"));
    }

    /**
     * @param im pImageMatix handle
     * @param filePath the save path of the jpeg file.
     * @param quality range [1,100]
     */
    static void dumpJpegFile(const pImageMatrix im,
        const string_t& filePath, uint8_t quality = 80) noexcept(false);

    /**
     * @param im pImageMatix handle
     * @param filePath the save path of the png file.
     */
    static void dumpPngFile(const pImageMatrix im,
        const string_t& filePath) noexcept(false);
};
