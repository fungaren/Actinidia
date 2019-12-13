#pragma once

#include "../libpng/png.h"
#include "../zlib/zlib.h"
extern "C" {
#include "../libjpeg/jpeglib.h"
}

class Matrix {
protected:
    Matrix() = default;
    Matrix(Matrix&) = delete;
    Matrix(Matrix&&) = default;
public:
    virtual uint16_t getWidth() const = 0;
    virtual uint16_t getHeight() const = 0;
    virtual uint32_t** getMatrix() const = 0;
};

class ImageMatrix : Matrix {
    friend class ImageMatrixFactory;

    ImageMatrix(uint32_t **table, uint16_t width, uint16_t height)
        : matrix(table), width(width), height(height) {}

    uint32_t** matrix;
    uint16_t width;
    uint16_t height;

public:
    ImageMatrix()
        : matrix(nullptr), width(0), height(0) {}
    ImageMatrix(ImageMatrix&) = delete;
    ImageMatrix(ImageMatrix&& src) 
        : matrix(src.matrix), width(src.width), height(src.height) {
        src.matrix = nullptr;
    }
    void operator=(ImageMatrix&& src) {
        ImageMatrix::~ImageMatrix();
        width = src.width;
        height = src.height;
        matrix = src.matrix;
        src.matrix = nullptr;
    }

    uint16_t getWidth() const { return width; };
    uint16_t getHeight() const { return height; };
    uint32_t** getMatrix() const { return matrix; };

    ~ImageMatrix() {
        if (matrix == nullptr)
            return;
        for (uint16_t y = 0; y < height; ++y)
            delete[] matrix[y];
        delete[] matrix;
    }
};

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

    static ImageMatrix fromPngFile(FILE *fp);
    static ImageMatrix readPngImpl(png_structp png_ptr, png_infop info_ptr);
    static ImageMatrix fromJpegFile(FILE *fp);
    static ImageMatrix readJpegImpl(jpeg_decompress_struct& cinfo, _jpeg_error_mgr& jerr);

    static void dumpJpegFile(const ImageMatrix& im, FILE *fp, uint8_t quality);
    static void dumpPngFile(const ImageMatrix& im, FILE *fp);

public:
    ImageMatrixFactory() = delete;

    struct mem_image {
        size_t size;
        size_t has_read = 8;
        void* addr;
        mem_image(void* pImageRes, size_t len)
            : addr(pImageRes), size(len) {}
    };

    static ImageMatrix createBufferImage(uint16_t width, uint16_t height,
        uint32_t bkgrdColor = 0xFFFFFFFF) noexcept(false);
    static ImageMatrix fromPngFile(const char* pngFile) noexcept(false);
    static ImageMatrix fromPngFile(const wchar_t* pngFile) noexcept(false);
    static ImageMatrix fromPngBuffer(mem_image *mp) noexcept(false);
#ifdef _WIN32
    static ImageMatrix fromPngResource(UINT nResID, LPCTSTR lpType, HMODULE hModule) noexcept(false);
#endif

    static ImageMatrix fromJpegFile(const char* jpegFile) noexcept(false);
    static ImageMatrix fromJpegFile(const wchar_t* jpegFile) noexcept(false);
    static ImageMatrix fromJpegBuffer(mem_image *mp) noexcept(false);
#ifdef _WIN32
    static ImageMatrix fromJpegResource(UINT nResID, LPCTSTR lpType, HMODULE hModule) noexcept(false);
#endif
    /// <summary>
    /// @param quality range [1,100]
    /// </summary>
    static void dumpJpegFile(const ImageMatrix& im, const char* filePath, uint8_t quality = 80) noexcept(false);
    static void dumpJpegFile(const ImageMatrix& im, const wchar_t* filePath, uint8_t quality = 80) noexcept(false);
    static void dumpPngFile(const ImageMatrix& im, const char* filePath) noexcept(false);
    static void dumpPngFile(const ImageMatrix& im, const wchar_t* filePath) noexcept(false);
};
