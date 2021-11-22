/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#ifdef _WIN32
    #include <windows.h>
    #undef max
#elif defined _GTK
    #include <iostream>
    #include <cstring>
#else
#error unsupported platform
#endif
#include <filesystem>

#include "Compress.h"
#include "ResourcePack.h"

#ifdef _WIN32

    static inline void _trace(std::wstring str) {
#ifdef _DEBUG
        OutputDebugStringW((str + L'\n').c_str());
#endif
    }
    static inline void _trace(std::string str) {
#ifdef _DEBUG
        OutputDebugStringA((str + '\n').c_str());
#endif
    }

#elif defined _GTK

    static inline void _trace(std::string str) {
#ifdef _DEBUG
        std::cout << str << std::endl;
#endif
    }

#else
#error unsupported platform
#endif

using namespace std::filesystem;

std::list<walk_layer> walkFolder(const string_t& dir)
{
    directory_iterator i = directory_iterator(dir);
    // The root directory is always "game"
    string_t dirName = ustr("game");
    std::string u8dirName = toUTF8(dirName);
    std::list<walk_layer> tree = {
        { entity{ TYPE_FOLDER, 0, (uint16_t)u8dirName.size(), 0 }, u8dirName, std::nullopt }
    };
    std::vector<std::tuple<path, directory_iterator, std::list<walk_layer>::iterator>> dirStack = {
        { dir, i, --tree.end() }
    };

this_dir:
    // Walk the directory
    while(i != directory_iterator())
    {
        if (dirStack.size() >= 64) {
            _trace("Directory is too deep");
            return {};
        }

        if (i->is_directory())
        {
            path __dir = static_cast<path>(*i);
            dirName = __dir.filename().to_ustr();
            if (dirName[0] == '.')
            {
                _trace(string_t(ustr("Skip hidden folder: ")) + dirName);
                ++i;
                continue;
            }
            _trace(dirName);

            u8dirName = toUTF8(dirName);
            tree.push_back({ entity{ TYPE_FOLDER, 0, (uint16_t)u8dirName.size(), 0 }, u8dirName, std::nullopt });
            i = directory_iterator(__dir);
            dirStack.push_back({ __dir, i, --tree.end() });
            goto this_dir;
        }
        else if (i->is_regular_file())
        {
            path __file = static_cast<path>(*i);
            string_t filePath = __file.to_ustr();
            string_t fileName = __file.filename().to_ustr();
            if (fileName[0] == '.')
            {
                _trace(string_t(ustr("skip hidden file: ")) + fileName);
                ++i;
                continue;
            }
            _trace(fileName);
            uintmax_t __size = i->file_size();
            if (__size >= UINT32_MAX)
            {
                _trace(ustr("The file is too large, skip"));
                ++i;
                continue;
            }
            uint32_t fileSize = (uint32_t)__size;
            std::string u8fileName = toUTF8(fileName);
            if ((__size = u8fileName.size()) >= UINT16_MAX)
            {
                _trace(ustr("The filename is too long, skip"));
                ++i;
                continue;
            }
            uint16_t nameSize = (uint16_t)__size;

            tree.push_back({ entity{ TYPE_FILE, 0, nameSize, fileSize }, u8fileName, filePath });

            // Increase the directory size
            entity& e = std::get<2>(dirStack.back())->e;
            e.dataSize += sizeof(entity) + nameSize + fileSize;
            ++i;
        } else {
            ++i; // Not a regular file or folder
        }
        continue;

    resume:
        // Get child directory entity
        entity& e = std::get<2>(dirStack.back())->e;
        // Close the child directory
        dirStack.pop_back();
        // Increase parent directory size
        std::get<2>(dirStack.back())->e.dataSize += sizeof(entity) + e.nameSize + e.dataSize;
        // Recover the iterator
        i = std::get<1>(dirStack.back());
        // Next file or directory
        ++i;
    }
    // Back to parent directory
    if (dirStack.size() != 1)
        goto resume;
    return tree;
}

bool generatePack(
    const string_t& destFile,
    const std::list<walk_layer>& tree,
    std::function<void(size_t filesize)> update_progress)
{
    std::stringstream buffer;
    buffer << MAGIC; // Magic
    for (auto& t : tree) {
        // Entity header
        buffer.write((char*)&t.e, sizeof(entity));
        // Name
        buffer << t.u8name;
        // File data
        if (t.path.has_value())
        {
            // Open file to read all data
            std::ifstream p(t.path.value(), std::ios::binary);
            // Get file size
            p.seekg(0, std::ios::end);
            std::streampos fsize = p.tellg();
            update_progress(fsize);
            if (fsize != 0)
            {
                p.seekg(0, std::ios::beg);
                buffer << p.rdbuf();
            }
        }
    }
    // Open output file
    std::ofstream dst(destFile, std::ios::binary);
    if (!dst)
    {
#ifdef _WIN32
        char error_str[1000];
        strerror_s(error_str, sizeof error_str, errno);
#elif defined _GTK
        char *error_str = strerror(errno);
#else
#error unsupported platform
#endif

        _trace(std::string("Failed to output file: ") + error_str);
        return false;
    }
    // Compress data and write to file
    int result = compress(buffer, dst, 9);
    if (result != 0)
    {
        _trace(zerr(result));
        return false;
    }
    return true;
}

static void parsePack(
    std::istream& res,
    std::function<void (const path&)> newFolder,
    std::function<void (std::istream&, const path&, uint32_t)> newFile) noexcept(false)
{
    path relativePath(".");
    std::vector<uint32_t> folder_size;

    for (size_t i = 0; i < sizeof(MAGIC) - 1; i++) {
        if (MAGIC[i] != res.get())
        {
            throw ustr_error(ustr("Invalid resouce pack"));
        }
    }

    do {
        entity e;
        res.read((char*)&e, sizeof(entity));

        // Read UTF-8 name
        std::string u8name;
        u8name.resize(e.nameSize);
        res.read(&u8name[0], e.nameSize);
        relativePath /= path(fromUTF8(u8name));

        if (e.type == TYPE_FOLDER)
        {
            newFolder(relativePath);

            // Empty folder
            if (e.dataSize == 0)
            {
                // Calc remaining bytes in this folder to be read
                if (!folder_size.empty())
                    folder_size.back() -= e.nameSize + sizeof(entity);

                // Back to parent folder
                relativePath = relativePath.parent_path();
            }
            else
            {
                // Not empty folder, calc remaining bytes in this folder to be read
                if (!folder_size.empty())
                    folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;

                // Enter subfolder
                folder_size.push_back(e.dataSize);
            }
        }
        else if (e.type == TYPE_FILE)
        {
            std::streampos p = res.tellg();

            // Dump file or read file
            newFile(res, relativePath, e.dataSize);

            // Handle next file
            res.seekg(p + std::streamoff(e.dataSize));

            // Back to parent folder
            relativePath = relativePath.parent_path();

            // Calc remaining bytes in this folder to be read
            folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;
        }
        else {
            throw ustr_error(ustr("Invalid resouce pack"));
        }

        while (!folder_size.empty() && folder_size.back() == 0)
        {
            // No more files/folders to load
            folder_size.pop_back();
            relativePath = relativePath.parent_path();
        }
    } while (relativePath != ustr("."));
}

bool extractPack(const string_t& resFile, const string_t& dir) noexcept(false)
{
    create_directory(dir);  // Extract files to this folder

    std::ifstream res(resFile, std::ios::binary);
    if (!res.good()) {
        throw ustr_error(ustr("Can not open file"));
    }
    std::stringstream buffer;
    int result = decompress(res, buffer);
    if (result != 0)
    {
        _trace(zerr(result));
        return false;
    }
    try {
        parsePack(
            buffer,
            // New folder: create folder
            [&dir](const path& relativePath) {
                create_directory(path{ dir } / relativePath);
            },
            // New file: dump file
            [&dir](std::istream& res, const path& relativePath, uint32_t dataSize) {
                std::ofstream out(path{ dir } / relativePath, std::ios::binary);

                char buffer[4096];
                while (dataSize > 4096) {
                    res.read(buffer, 4096);
                    out.write(buffer, 4096);
                    dataSize -= 4096;
                }
                res.read(buffer, dataSize);
                out.write(buffer, dataSize);
            }
        );
    }
    catch (ustr_error& e) {
        _trace(e.what());
        return false;
    }
    return true;
}

pResourcePack ResourcePack::parsePack(const string_t& pathname) noexcept(false)
{
    std::ifstream res(pathname, std::ios::binary);
    if (!res.good()) {
        throw ustr_error(ustr("Can not open file"));
    }
    std::stringstream buffer;
    int result = decompress(res, buffer);
    if (result != 0)
    {
        throw ustr_error(zerr(result));
    }
    pResourcePack pack = new ResourcePack();
    ::parsePack(
        buffer,
        // New folder
        [](const path& relativePath) { },
        // New file: read into RAM
        [pack](std::istream& res, const path& relativePath, uint32_t dataSize) {
            uint8_t* data = new uint8_t[dataSize];
            res.read((char*)data, dataSize);
            pack->list.emplace_back(relativePath.to_ustr(), data, dataSize);
        }
    );

    return pack;
}

bool ResourcePack::readResource(const string_t& pathname, uint8_t** p, uint32_t* size) const
{
    // The pathname should be like "./path/to/file.jpg"
    if (pathname[0] != '.')
        return false;

    // Converts all directory separators in the generic-format view of the
    // path to the preferred directory separator.
    const string_t pathname_ = static_cast<path>(pathname).make_preferred().to_ustr();

    auto i = std::find_if(list.begin(), list.end(),
        [&pathname_](const ResourceFile& i) {
            return (i.path == pathname_);
        }
    );

    if (i != list.end()) {
        // Found target file, set the data pointer & size
        *p = i->data;
        *size = i->dataSize;
        return true;
    }
    else return false;
}
