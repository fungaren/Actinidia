/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
    // #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef max
#elif defined _GTK
    #include <iostream>
#else
#error unsupported platform
#endif

#include "ImageMatrix.h"

pImageMatrix ImageMatrixFactory::createBufferImage(uint16_t width, uint16_t height, uint32_t bkgrdColor)
{
    if (width == 0 || height == 0)
        throw ustr_error(ustr("Width and height cannot be zero"));

    // Allocate memory space for image matrix
    uint32_t *table;
    try {
        table = new uint32_t[(size_t)width * height];
    }
    catch (std::bad_alloc&) {
        throw ustr_error(ustr("Cannot allocate memory"));
    }
    for (size_t i = 0; i < (size_t)width * height; i++)
        table[i] = bkgrdColor;
    return std::shared_ptr<ImageMatrix>(new ImageMatrix(table, width, height));
}

pImageMatrix ImageMatrixFactory::fromPngFile(const string_t& pngFile)
{
    FILE *fp = openfile(pngFile, ustr("rb"));
    try {
        pImageMatrix im = fromPngFile(fp);
        fclose(fp);
        return im;
    }
    catch (...) {
        fclose(fp);
        throw;
    }
}

pImageMatrix ImageMatrixFactory::fromPngFile(FILE *fp)
{
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    // Error capture
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Failed to load image"));
    }

    // Check if the PNG image is valid
    char buf[8];
    if (fread(buf, 1, 8, fp) < 8) { // read PNG sign
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Failed to read file"));
    }
    if (png_sig_cmp((png_bytep)buf, (png_size_t)0, 8) != 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Invalid PNG image"));
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    return readPngImpl(png_ptr, info_ptr);
}

pImageMatrix ImageMatrixFactory::fromPngBuffer(mem_image* mp)
{
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    // Error capture
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Failed to load image"));
    }

    // Check if the PNG image is valid
    if (mp->size < 8) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Failed to read file"));
    }
    if (png_sig_cmp((png_bytep)mp->addr, (png_size_t)0, 8) != 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Invalid PNG image"));
    }

    png_set_read_fn(png_ptr, mp, png_read_data_fn);
    png_set_sig_bytes(png_ptr, 8);

    return readPngImpl(png_ptr, info_ptr);
}

#ifdef _WIN32
pImageMatrix ImageMatrixFactory::fromPngResource(UINT nResID, LPCTSTR lpType, HMODULE hModule)
{
    HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(nResID), lpType);
    if (hRsrc == NULL)
        throw ustr_error(ustr("Failed to find resource"));

    HGLOBAL hImgData = LoadResource(hModule, hRsrc);
    if (hImgData == NULL)
        throw ustr_error(ustr("Failed to load resource"));

    LPVOID lpVoid = LockResource(hImgData);
    DWORD dwSize = SizeofResource(hModule, hRsrc);

    auto mp = ImageMatrixFactory::mem_image(lpVoid, dwSize);
    try {
        pImageMatrix im = fromPngBuffer(&mp);
        UnlockResource(hImgData);
        FreeResource(hImgData);
        return im;
    }
    catch (...) {
        UnlockResource(hImgData);
        FreeResource(hImgData);
        throw;
    }
}
#elif defined _GTK
#else
#error unsupported platform
#endif

void ImageMatrixFactory::png_read_data_fn(png_structp png_ptr, png_bytep dest, png_size_t length)
{
    mem_image* mp = (mem_image*)png_get_io_ptr(png_ptr);
#ifdef _WIN32
    memcpy_s(dest, length, (uint8_t*)mp->addr + mp->has_read, length);
#elif defined _GTK
    memcpy(dest, (uint8_t*)mp->addr + mp->has_read, length);
#else
#error unsupported platform
#endif
    mp->has_read += length;
}

pImageMatrix ImageMatrixFactory::readPngImpl(png_structp png_ptr, png_infop info_ptr)
{
    // Read PNG info
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
    uint8_t color_type = png_get_color_type(png_ptr, info_ptr);
    uint32_t w = png_get_image_width(png_ptr, info_ptr);
    uint32_t h = png_get_image_height(png_ptr, info_ptr);
    if (w >= UINT16_MAX || h >= UINT16_MAX) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Image size is too large"));
    }

    // allocate memory space for image matrix
    uint32_t *table;
    try {
        table = new uint32_t[(size_t)w*h];
    }
    catch (std::bad_alloc&) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Cannot allocate memory"));
    }

    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
    switch (color_type) {
    case PNG_COLOR_TYPE_RGBA:
        for (uint16_t y = 0; y < h; ++y) {
            for (uint16_t x = 0; x < w; ++x) {
                png_bytep row = row_pointers[y];
                // read RGBA channels and convert to ARGB
                uint32_t argb =
                    (row[4 * x + 3] << 8 * 3) | // Alpha (In fact it's opacity)
                    (row[4 * x + 0] << 8 * 2) | // Red
                    (row[4 * x + 1] << 8 * 1) | // Green
                    (row[4 * x + 2] << 8 * 0);  // Blue
                table[y * w + x] = argb;
            }
        }
        break;
    case PNG_COLOR_TYPE_RGB:
        for (uint16_t y = 0; y < h; ++y) {
            for (uint16_t x = 0; x < w; ++x) {
                png_bytep row = row_pointers[y];
                // read RGB channels and convert to ARGB
                uint32_t argb = 0xFF000000  |   // Alpha (In fact it's opacity)
                    (row[3 * x + 0] << 8 * 2) | // Red
                    (row[3 * x + 1] << 8 * 1) | // Green
                    (row[3 * x + 2] << 8 * 0);  // Blue
                table[y * w + x] = argb;
            }
        }
        break;
    default:
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        throw ustr_error(ustr("Unknown color type"));
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    return std::shared_ptr<ImageMatrix>(new ImageMatrix(table, w, h));
}

void ImageMatrixFactory::dumpPngFile(const pImageMatrix im, const string_t& filePath)
{
    FILE *fp = openfile(filePath, ustr("wb"));
    try {
        dumpPngFile(im, fp);
        fclose(fp);
    }
    catch (...) {
        fclose(fp);
        throw;
    }
}

void ImageMatrixFactory::dumpPngFile(const pImageMatrix im, FILE *fp)
{
    png_structp png_ptr;
    png_infop info_ptr;
    //png_colorp palette;

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == NULL)
        throw ustr_error(ustr("Failed to create write struct"));

    /* Allocate/initialize the image information data.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        throw ustr_error(ustr("Failed to create info struct"));
    }

    /* Set up error handling.  REQUIRED if you aren't supplying your own
     * error handling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* If we get here, we had a problem writing the file. */
        png_destroy_write_struct(&png_ptr, &info_ptr);
        throw ustr_error(ustr("Failed to write png file"));
    }

    /* One of the following I/O initialization functions is REQUIRED. */

    /* I/O initialization method 1 */
    /* Set up the output control if you are using standard C streams. */
    png_init_io(png_ptr, fp);

    /* I/O initialization method 2 */
    /* If you are using replacement write functions, instead of calling
     * png_init_io(), you would call:
     */
    //png_set_write_fn(png_ptr, (void *)user_io_ptr, user_write_fn,
    //  user_IO_flush_function);
    /* where user_io_ptr is a structure you want available to the callbacks. */

    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8 or 16, but valid values also depend on
     * the color_type selected.  color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE.
     * REQUIRED.
     */
    png_set_IHDR(png_ptr, info_ptr, im->getWidth(), im->getHeight(), 8,
        PNG_COLOR_TYPE_RGBA , PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    /* Set up the transformations you want.  Note that these are
     * all optional.  Only call them if you want them.
     */

    /* Flip BGR pixels to RGB. */
    png_set_bgr(png_ptr);

    // Generate row pointers of the pixel data.
    uint32_t **rows = new uint32_t*[im->getHeight()];
    for (uint32_t i=0; i < im->getHeight(); ++i)
        rows[i] = im->getMatrix() + i * im->getWidth();

    png_set_rows(png_ptr, info_ptr, (png_bytepp)rows);

    /* Set the palette if there is one.  REQUIRED for indexed-color images. */
    //palette = (png_colorp)png_malloc(png_ptr,
    //  PNG_MAX_PALETTE_LENGTH * (sizeof(png_color)));
    /* ... Set palette colors ... */
    //png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
    /* You must not free palette here, because png_set_PLTE only makes a link
     * to the palette that you allocated.  Wait until you are about to destroy
     * the png structure.
     */

    /* This is the easy way.  Use it if you already have all the
     * image info living in the structure.  You could "|" many
     * PNG_TRANSFORM flags into the png_transforms integer here.
     */
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* If you png_malloced a palette, free it here.
     * (Don't free info_ptr->palette, as shown in versions 1.0.5m and earlier of
     * this example; if libpng mallocs info_ptr->palette, libpng will free it).
     * If you allocated it with malloc() instead of png_malloc(), use free()
     * instead of png_free().
     */
    //png_free(png_ptr, palette);
    //palette = NULL;
    delete[] rows;

    /* Whenever you use png_free(), it is a good idea to set the pointer to
    * NULL in case your application inadvertently tries to png_free() it
    * again.  When png_free() sees a NULL it returns without action, avoiding
    * the double-free problem.
    */

    /* Clean up after the write, and free any allocated memory. */
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

pImageMatrix ImageMatrixFactory::fromJpegFile(const string_t& jpegFile)
{
    FILE *fp = openfile(jpegFile, ustr("rb"));
    try {
        pImageMatrix im = fromJpegFile(fp);
        fclose(fp);
        return im;
    }
    catch (...) {
        fclose(fp);
        throw;
    }
}

void ImageMatrixFactory::jpeg_error_exit(j_common_ptr cinfo) {
    _jpeg_error_mgr *myerr = (_jpeg_error_mgr*)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

pImageMatrix ImageMatrixFactory::fromJpegFile(FILE *fp)
{
    jpeg_decompress_struct cinfo;
    _jpeg_error_mgr jerr;

    /* Step 1: allocate and initialize JPEG decompression object */
    cinfo.err = jpeg_std_error(&jerr.pub);

    jerr.pub.error_exit = jpeg_error_exit;

    /* Establish the setjmp return context for jpeg_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        // fetch error string
        char buffer[JMSG_LENGTH_MAX];
        (cinfo.err->format_message) ((j_common_ptr)&cinfo, buffer);

        jpeg_destroy_decompress(&cinfo);
        throw ustr_error(fromUTF8(buffer));
    }

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);

    /* Step 2: specify data source (eg, a file) */
    jpeg_stdio_src(&cinfo, fp);

    return readJpegImpl(cinfo, jerr);
}

pImageMatrix ImageMatrixFactory::fromJpegBuffer(mem_image *mp)
{
    jpeg_decompress_struct cinfo;
    _jpeg_error_mgr jerr;

    /* Step 1: allocate and initialize JPEG decompression object */
    cinfo.err = jpeg_std_error(&jerr.pub);

    jerr.pub.error_exit = jpeg_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        // fetch error string
        char buffer[JMSG_LENGTH_MAX];
        (cinfo.err->format_message) ((j_common_ptr)&cinfo, buffer);

        jpeg_destroy_decompress(&cinfo);
        throw ustr_error(fromUTF8(buffer));
    }
    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);

    /* Step 2: specify data source (eg, a file) */
    jpeg_mem_src(&cinfo, (uint8_t*)mp->addr, (unsigned long)mp->size);

    return readJpegImpl(cinfo, jerr);
}

#ifdef _WIN32

pImageMatrix ImageMatrixFactory::fromJpegResource(UINT nResID, LPCTSTR lpType, HMODULE hModule)
{
    HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(nResID), L"lpType");
    if (hRsrc == NULL)
        throw ustr_error(ustr("Failed to find resource"));

    HGLOBAL hImgData = LoadResource(hModule, hRsrc);
    if (hImgData == NULL)
        throw ustr_error(ustr("Failed to load resource"));

    LPVOID lpVoid = LockResource(hImgData);
    DWORD dwSize = SizeofResource(hModule, hRsrc);

    auto mp = ImageMatrixFactory::mem_image(lpVoid, dwSize);
    try {
        pImageMatrix im = fromJpegBuffer(&mp);
        UnlockResource(hImgData);
        FreeResource(hImgData);
        return im;
    }
    catch (...) {
        UnlockResource(hImgData);
        FreeResource(hImgData);
        throw;
    }
}
#elif defined _GTK
#else
#error unsupported platform
#endif

pImageMatrix ImageMatrixFactory::readJpegImpl(jpeg_decompress_struct& cinfo, _jpeg_error_mgr& jerr)
{
    JSAMPARRAY buffer;            /* Output row buffer */
    int row_stride;               /* physical row width in output buffer */

    /* Step 3: read file parameters with jpeg_read_header() */
    (void)jpeg_read_header(&cinfo, TRUE);

    /* Step 4: set parameters for decompression */

    /* In this example, we don't need to change any of the defaults set by
    * jpeg_read_header(), so we do nothing here.
    */

    /* Step 5: Start decompressor */
    (void)jpeg_start_decompress(&cinfo);

    // We only support RGB color now
    if (cinfo.output_components != 3) {
        jpeg_destroy_decompress(&cinfo);
        throw ustr_error(ustr("Unknown color type"));
    }

    /* We may need to do some setup of our own at this point before reading
    * the data.  After jpeg_start_decompress() we have the correct scaled
    * output image dimensions available, as well as the output colormap
    * if we asked for color quantization.
    * In this example, we need to make an output work buffer of the right size.
    */
    /* JSAMPLEs per row in output buffer */
    row_stride = cinfo.output_width * cinfo.output_components;

    if (cinfo.output_width >= UINT16_MAX || cinfo.output_height >= UINT16_MAX) {
        jpeg_destroy_decompress(&cinfo);
        throw ustr_error(ustr("Image size is too large"));
    }
    uint16_t w = cinfo.output_width;
    uint16_t h = cinfo.output_height;

    /* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    // allocate memory space for image matrix
    uint32_t *table;
    try {
        table = new uint32_t[(size_t)w*h];
    }
    catch (std::bad_alloc&) {
        jpeg_destroy_decompress(&cinfo);
        throw ustr_error(ustr("Cannot allocate memory"));
    }

    /* Step 6: while (scan lines remain to be read) */
    /*           jpeg_read_scanlines(...); */

    /* Here we use the library's state variable cinfo.output_scanline as the
    * loop counter, so that we don't have to keep track ourselves.
    */
    while (cinfo.output_scanline < cinfo.output_height) {
        /* jpeg_read_scanlines expects an array of pointers to scanlines.
        * Here the array is only one element long, but you could ask for
        * more than one scanline at a time if that's more convenient.
        */
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        /* Copy to destination */
        for (uint16_t x = 0; x < w; x++) {
            uint32_t argb = 0xFF000000  |       // Alpha (In fact it's opacity)
                buffer[0][x * 3 + 0] << 8 * 2 | // Red
                buffer[0][x * 3 + 1] << 8 * 1 | // Green
                buffer[0][x * 3 + 2] << 8 * 0;  // Blue

            /*if (rotate180) {
                table[(h - cinfo.output_scanline) * w + (w - x - 1)] = argb;
            } else {*/
            table[(cinfo.output_scanline - 1) * w + x] = argb;
            //}
        }
    }

    /* Step 7: Finish decompression */

    (void)jpeg_finish_decompress(&cinfo);
    /* We can ignore the return value since suspension is not possible
    * with the stdio data source.
    */

    /* Step 8: Release JPEG decompression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

    /* After finish_decompress, we can close the input file.
    * Here we postpone it until after no more JPEG errors are possible,
    * so as to simplify the setjmp error logic above.  (Actually, I don't
    * think that jpeg_destroy can do an error exit, but why assume anything...)
    */
    return std::shared_ptr<ImageMatrix>(new ImageMatrix(table, w, h));
}

void ImageMatrixFactory::dumpJpegFile(const pImageMatrix im, const string_t& filePath, uint8_t quality)
{
    if (quality > 100)
        throw ustr_error(ustr("Quality range [1,100]"));
    FILE *fp = openfile(filePath, ustr("wb"));
    try {
        dumpJpegFile(im, fp, quality);
        fclose(fp);
    }
    catch (...) {
        fclose(fp);
        throw;
    }
}

void ImageMatrixFactory::dumpJpegFile(const pImageMatrix im, FILE *fp, uint8_t quality)
{
    /* This struct contains the JPEG compression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     * It is possible to have several such structures, representing multiple
     * compression/decompression processes, in existence at once.  We refer
     * to any one struct (and its associated working data) as a "JPEG object".
     */
    struct jpeg_compress_struct cinfo;
    /* This struct represents a JPEG error handler.  It is declared separately
     * because applications often want to supply a specialized error handler
     * (see the second half of this file for an example).  But here we just
     * take the easy way out and use the standard error handler, which will
     * print a message on stderr and call exit() if compression fails.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    struct jpeg_error_mgr jerr;
    /* More stuff */
    JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
    int row_stride;               /* physical row width in image buffer */

    /* Step 1: allocate and initialize JPEG compression object */

    /* We have to set up the error handler first, in case the initialization
     * step fails.  (Unlikely, but it could happen if you are out of memory.)
     * This routine fills in the contents of struct jerr, and returns jerr's
     * address which we place into the link field in cinfo.
     */
    cinfo.err = jpeg_std_error(&jerr);
    /* Now we can initialize the JPEG compression object. */
    jpeg_create_compress(&cinfo);

    /* Step 2: specify data destination (eg, a file) */
    /* Note: steps 2 and 3 can be done in either order. */

    /* Here we use the library-supplied code to send compressed data to a
     * stdio stream.  You can also write your own code to do something else.
     * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
     * requires it in order to write binary files.
     */
    jpeg_stdio_dest(&cinfo, fp);

    /* Step 3: set parameters for compression */

    /* First we supply a description of the input image.
     * Four fields of the cinfo struct must be filled in:
     */
    cinfo.image_width = im->getWidth();    /* image width and height, in pixels */
    cinfo.image_height = im->getHeight();
    cinfo.input_components = 4;           /* # of color components per pixel */
    cinfo.in_color_space = JCS_EXT_BGRA;  /* colorspace of input image */

    // ARGB color on little-endian computers will be read as BGRA so we use JCS_EXT_BGRA

    /* Now use the library's routine to set default compression parameters.
     * (You must set at least cinfo.in_color_space before calling this,
     * since the defaults depend on the source color space.)
     */
    jpeg_set_defaults(&cinfo);
    /* Now you can set any non-default parameters you wish to.
     * Here we just illustrate the use of quality (quantization table) scaling:
     */
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

    /* Step 4: Start compressor */

    /* TRUE ensures that we will write a complete interchange-JPEG file.
     * Pass TRUE unless you are very sure of what you're doing.
     */
    jpeg_start_compress(&cinfo, TRUE);

    /* Step 5: while (scan lines remain to be written) */
    /*           jpeg_write_scanlines(...); */

    /* Here we use the library's state variable cinfo.next_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     * To keep things simple, we pass one scanline per call; you can pass
     * more if you wish, though.
     */
    row_stride = im->getWidth() * 4; /* JSAMPLEs per row in image_buffer */
    (void)row_stride; /* not used */

    while (cinfo.next_scanline < cinfo.image_height) {
        /* jpeg_write_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could pass
         * more than one scanline at a time if that's more convenient.
         */
        row_pointer[0] = (JSAMPROW)
            (im->getMatrix() + cinfo.next_scanline * im->getWidth());
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    /* Step 6: Finish compression */
    jpeg_finish_compress(&cinfo);

    /* Step 7: release JPEG compression object */
    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_compress(&cinfo);
}
