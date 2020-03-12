#include "pch.h"
#include "ResourcePack.h"
#include "Compress.h"

inline void _trace(std::wstring str) {
#ifdef _DEBUG
    OutputDebugString((str + L'\n').c_str());
#endif
}

using namespace std::filesystem;

walk_result_t walkFolder(const path& resDirectory)
{
    std::vector<path> folder_path = { resDirectory };
    // store meta info to be output
    walk_result_t queue;
    // store iterator of parent folder
    std::vector<std::pair<directory_iterator, walk_result_t::iterator>> parent_iterator;

    directory_iterator i;
    uint8_t depth = 0;
recursive:
    // enter the directory, and the root directory is always "game"
    std::string foldername = (depth == 0) ? "game" : folder_path.back().filename().u8string();
    i = directory_iterator(folder_path.back());

    // write entity + file name
    entity e{ TYPE_FOLDER, 0, (uint16_t)foldername.size(), 0 };
    // store directory_iterator
    queue.push_back({ e, foldername, std::nullopt });
    parent_iterator.push_back({ i,--queue.end() });

    ++depth;

    // walk the directory
    while(i != directory_iterator())
    {
        if (depth == 64)
            return queue;       // not support depth > 64

        if (i->is_directory())
        {
            if (static_cast<path>(*i).filename().wstring().front() == L'.') {
                ++i; // skip hidden folder (eg., .git)
                continue;
            }
            _trace(static_cast<path>(*i).filename());
            folder_path.push_back(*i);
            goto recursive;
        resume:
            // update parent folder size
            auto h = std::get<0>(*parent_iterator.rbegin()->second);
            parent_iterator.pop_back();
            std::get<0>(*(parent_iterator.rbegin()->second)).dataSize += 
                sizeof(entity) + h.nameSize + h.dataSize;
            // restore
            i = parent_iterator.back().first;
            ++i;
        }
        else if (i->is_regular_file())
        {
            path _filename = static_cast<path>(*i).filename();
            if (_filename.wstring().front() == L'.') {
                ++i; // skip hidden folder (eg., .git)
                continue;
            }
            _trace(_filename);
            // get utf-8 encoded file name
            std::string filename = _filename.u8string();
            uint32_t file_size = (uint32_t)i->file_size();
            uint16_t name_size = (uint16_t)filename.size();

            // write entity + file name + data
            entity e{ TYPE_FILE, 0, name_size, file_size };
            queue.push_back({ e, filename, static_cast<path>(*i) });

            // update parent folder size
            auto h = parent_iterator.rbegin()->second;
            std::get<0>(*h).dataSize += sizeof(entity) + name_size + file_size;
            ++i;
        }
    }
    // back to parent directory
    folder_path.pop_back();
    if (!folder_path.empty()) {
        --depth;
        goto resume;
    }
    return queue;
}

bool generatePack(const std::string& destFile,
    const walk_result_t& queue,
    std::function<void(size_t filesize)> update_progress)
{
    std::stringstream buffer;
    buffer << MAGIC; // Magic
    for (auto& t : queue) {
        buffer.write((char*)&std::get<0>(t), sizeof entity); // Entity
        buffer << std::get<1>(t); // Name
        if (std::get<2>(t).has_value())
        {
            std::ifstream p(std::get<2>(t).value(), std::ios::binary); // Input stream
            p.seekg(0, std::ios::end);
            std::streampos pos = p.tellg(); // get file size
            update_progress(pos); // the callback update packing progress
            if (pos != 0)
            {
                p.seekg(0, std::ios::beg);
                buffer << p.rdbuf(); // Data
            }
        }
    }
    std::ofstream dst(destFile, std::ios::binary);
    if (!dst) {
        char error_str[1024];
        strerror_s(error_str, sizeof error_str, errno);
        std::cerr << error_str << '\n';
        return false;
    }
    int result = compress(buffer, dst, 9);
    if (result != 0)
    {
        _trace(zerrw(result));
        return false;
    }
    return true;
}

void parsePack(std::istream& res,
    std::function<void (const path&)> new_folder,
    std::function<void (std::istream&, const path&, uint32_t)> new_file) noexcept(false)
{
    path relativePath(".");
    std::vector<uint32_t> folder_size;

    for (int i = 0; i < sizeof MAGIC - 1; i++) {
        if (MAGIC[i] != res.get())
        {
            std::runtime_error e("Invalid resouce pack");
            throw(e);
        }
    }
    
    do {
        entity e;
        res.read((char*)&e, sizeof(entity));

        // read name
        char* name = new char[(size_t)e.nameSize + 1];
        res.read(name, e.nameSize);
        name[e.nameSize] = '\0';
        try {
            relativePath /= u8path(name);
        }
        catch (std::exception e) {
            delete[] name;
            throw e;
        }
        delete[] name;

        if (e.type == TYPE_FOLDER)
        {
            new_folder(relativePath);

            // empty folder
            if (e.dataSize == 0) {

                // calc remaining bytes in this folder to be read
                if (!folder_size.empty())
                    folder_size.back() -= e.nameSize + sizeof(entity);

                // back to parent folder
                relativePath = relativePath.parent_path();
            }
            else { // not empty folder

                // calc remaining bytes in this folder to be read
                if (!folder_size.empty())
                    folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;

                // enter subfolder
                folder_size.push_back(e.dataSize);
            }
        }
        else if (e.type == TYPE_FILE)
        {
            // calc remaining bytes in this folder to be read
            folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;
            
            if (e.dataSize != 0)
            {
#if _DEBUG
                std::streampos p = res.tellg();

                // dump file or read file
                new_file(res, relativePath, e.dataSize);

                // you must consume these bytes
                _ASSERT(res.tellg() == p + std::streamoff(e.dataSize));
#else
                // dump file or read file
                new_file(res, relativePath, e.dataSize);
#endif /* _DEBUG */
            }

            // back to parent folder
            relativePath = relativePath.parent_path();
        }

        while (!folder_size.empty() && folder_size.back() == 0) {
            // no more files/folders to load
            folder_size.pop_back();
            relativePath = relativePath.parent_path();
        }
    } while (relativePath != ".");
}

bool extractPack(const path& resFile) noexcept(false)
{
    std::ostringstream extract_dir_name;
    extract_dir_name << "extract_" << time(NULL);
    path folder_path = resFile.parent_path() / extract_dir_name.str();

    create_directory(folder_path);  // extract files to this folder

    std::ifstream res(resFile, std::ios::binary);
    if (!res.good()) {
        std::runtime_error e("Can not open file");
        throw e;
    }
    std::stringstream buffer;
    int result = decompress(res, buffer);
    if (result != 0)
    {
        _trace(zerrw(result));
        return false;
    }
    try {
        parsePack(
            buffer,
            // new folder: create folder
            [&folder_path](const path& relativePath) {
                create_directory(folder_path / relativePath);
            },
            // new file: dump file
            [&folder_path](std::istream& res, const path& relativePath, uint32_t dataSize) {
                std::ofstream out(folder_path / relativePath, std::ios::binary);

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
    catch (std::exception) {
        return false;
    }
    return true;
}

pResourcePack ResourcePack::parsePack(const path& resFile) noexcept(false)
{
    std::ifstream res(resFile, std::ios::binary);
    if (!res.good()) {
        std::runtime_error e("Can not open file");
        throw e;
    }
    std::stringstream buffer;
    int result = decompress(res, buffer);
    if (result != 0)
    {
        std::runtime_error e(zerr(result));
        throw e;
    }
    pResourcePack pack = new ResourcePack();
    ::parsePack(
        buffer,
        // new folder
        [](const path& relativePath) { },
        // new file: read into RAM
        [pack](std::istream& res, const path& relativePath, uint32_t dataSize) {
            uint8_t* data = new uint8_t[dataSize];
            res.read((char*)data, dataSize);
            pack->list.emplace_back(relativePath.u8string(), data, dataSize);
        }
    );

    return pack;
}

bool ResourcePack::readResource(std::string pathname, uint8_t** p, uint32_t* size) const
{
    // the pathname should be like "./path/to/file.jpg"
    if (pathname[0] != L'.')
        return false;

    // Converts all directory separators in the generic-format view of the
    // path to the preferred directory separator. 
    const std::string pathname_ = static_cast<path>(pathname).make_preferred().string();

    auto i = std::find_if(list.begin(), list.end(), 
        [&pathname_](const ResourceFile& i) {
            return (i.path == pathname_);
        }
    );

    if (i != list.end()) {
        // found target file, set the data pointer & size
        *p = i->data;
        *size = i->dataSize;
        return true;
    }
    else return false;
}
