/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#include <getopt.h>
#include <climits>

#include <iostream>
#include <vector>
#include <filesystem>

#include "Common/PiCanvas.h"
#include "Common/ResourcePack.h"
#include "Common/ImageMatrix.h"

using namespace std::filesystem;

static void deploy_pack(const string_t& dstfile, const string_t& dir)
{
    std::cout << "Packing folder: " << dir << '\n';
    std::cout << std::unitbuf << "Progress: [                    ]"; // 20
    std::list<walk_layer> tree = walkFolder(dir);
    size_t totalSize = tree.front().e.dataSize;
    size_t count = 0;
    size_t progress = 0; // 0-20
    if (!generatePack(dstfile, tree, [totalSize, &progress, &count](size_t filesize) {
            count += filesize;
            size_t prog = count * 20 / totalSize;
            // update progress bar
            if (prog > progress)
            {
                progress = prog;
                std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"; // 21
                for (size_t i=0; i < progress; ++i)
                    std::cout << '=';
                for (size_t i=progress; i < 20; ++i)
                    std::cout << ' ';
                std::cout << ']';
            }
        }))
        std::cout << "\nFailed to generate pack.\n";
    std::cout << '\n' << std::nounitbuf;
}

static void deploy_unpack(const string_t& resfile, const string_t& dir)
{
    std::cout << "Unpacking resource file: " << resfile << '\n';
    path outputDir{dir};
    if (!exists(outputDir))
    {
        std::cerr << "Output directory does not exist.\n";
        return;
    }
    if (!extractPack(resfile, dir))
        std::cout << "Unsupported resource package.\n";
}

static void deploy_image_contatenate(const string_t& dstfile, const string_t& dir)
{
    std::vector<string_t> files;
    std::vector<pImageMatrix> imgs;
    uint32_t width = 0, total_height = 0;
    std::cout << "Concatenate images in directory: " << dir << '\n';
    path dir_path = dir;
    for (directory_iterator i{dir}; i != directory_iterator{}; ++i)
    {
        if (i->is_regular_file())
            files.push_back(dir_path / static_cast<path>(*i).filename());
    }
    // Sort files by their names
    std::sort(files.begin(), files.end(), [](const string_t& a, const string_t& b) -> bool{
        size_t p1 = a.rfind('\\');
        string_t n1 = (p1 == string_t::npos ? a : a.substr(p1 + 1));
        size_t p2 = b.rfind('\\');
        string_t n2 = (p2 == string_t::npos ? b : b.substr(p2 + 1));
        return n1.compare(n2) < 0;
    });
    std::cout << std::unitbuf;
    for (auto& file : files) {
        imgs.emplace_back(ImageMatrixFactory::fromImageFile(file));
        if (width == 0)
            width = imgs.back()->getWidth();
        else if (imgs.back()->getWidth() != width)
            throw std::runtime_error("Images width don't match.\n"\
                "Only images with the same width can be concatenated.");
        total_height += imgs.back()->getHeight();
        if (total_height > UINT16_MAX)
            throw std::runtime_error("Image height is too large. \n"\
                "Reduce the number of images and try again.");
        std::cout << '.';
    }
    auto temp = ImageMatrixFactory::createBufferImage(width, total_height, Canvas::Constant::alpha);
    uint16_t y = 0;
    for (const pImageMatrix i : imgs) {
        PiCanvas::blend(temp, i, 0, y, 0xFF);
        y += i->getHeight();
        std::cout << ':';
    }
    std::cout << std::nounitbuf;
    std::cout << "\nOutputing...\n";
    string_t extName = dstfile.substr(dstfile.rfind('.') + 1);
    if (extName == "png" || extName == "PNG")
        ImageMatrixFactory::dumpPngFile(temp, dstfile.c_str());
    else
        ImageMatrixFactory::dumpJpegFile(temp, dstfile.c_str(), 100);
    std::cout << "Done\n";
}

static void help(char* argv[])
{
    std::cout <<
    "Usage: " << argv[0] << " [OPTIONS]\n" \
    "    -p    Pack folder into resource file\n" \
    "    -u    Unpack resource file\n" \
    "    -c    Concatenate images in the directory into a single image\n" \
    "    -o    Output path\n";
}

int main(int argc, char* argv[])
{
    string_t output_path, arg;
    int opt, action = '?';
    while ((opt = getopt(argc, argv, "p:u:c:o:")) != -1)
    {
        switch (opt)
        {
        case 'p':
        case 'u':
        case 'c':
            action = opt;
            arg = optarg;
            break;
        case 'o':
            output_path = optarg;
            break;
        default:
            help(argv);
            return 1;
        }
    }
    if (output_path.empty() || arg.empty()) {
        help(argv);
        return 1;
    }
    try {
        switch (action)
        {
        case 'p':
            deploy_pack(output_path, arg);
            break;
        case 'u':
            deploy_unpack(arg, output_path);
            break;
        case 'c':
            deploy_image_contatenate(output_path, arg);
            break;
        default:
            return 1;
        }
    }
    catch (ustr_error &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
