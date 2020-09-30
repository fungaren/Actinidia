/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#pragma once
#include <optional>
#include <fstream>
#include <sstream>
#include <queue>
#include <list>
#include <functional>
#ifndef _ASSERT
    #define _ASSERT(...)
#endif /* _ASSERT */

/*
    resource package format:
    - entity + foldername
        - entity + filename + data
        - entity + foldername + data
            - entity + filename + data
            - entity + filename + data
            ...
        ...
    ...

*/
const uint8_t TYPE_FOLDER = 0x01;
const uint8_t TYPE_FILE = 0x00;
const char MAGIC[] = "ACTINIDIA";

struct entity {
    uint8_t type;       // entity type
    uint8_t reserved;   // not used, maybe attribute
    uint16_t nameSize;  // do not support filename length > 65535
    //uint32_t checksum;
    uint32_t dataSize;  // do not contains sizeof(entity) and name
                        // do not support filesize > 4GB
};

typedef std::list<std::tuple<entity, std::string, std::optional<std::filesystem::path>>> walk_result_t;
walk_result_t walkFolder(const std::filesystem::path& resDirectory);
bool generatePack(const std::string& destFile,
    const walk_result_t& queue,
    std::function<void(size_t)> update_progress);
bool extractPack(const std::filesystem::path& resFile);

/*
    Resource file in memory:
    - /filename
    - /foldername/filename
    - /foldername/filename
    - /filename
    - /foldername/foldername/filename
    - /filename
    - /filename
*/

class ResourcePack {
    // All resources are load to memory, make sure enough capcity of the memory.
    struct ResourceFile {
        std::string path;
        uint8_t* data;
        uint32_t dataSize;
        ResourceFile(std::string path, uint8_t* data, uint32_t dataSize) :
            path(path),
            data(data),
            dataSize(dataSize)
        {
            _ASSERT(data != nullptr);
            _ASSERT(dataSize != 0);
        }
    };
    std::vector<ResourceFile> list;
    ResourcePack() {};
public:
    ResourcePack(ResourcePack&) = delete;
    ~ResourcePack() {
        for (auto& i : list)
            delete i.data;
    }
    typedef ResourcePack* pResourcePack;

    // Open a package and construct a ResourcePack instance.
    // Will throw an exception for any error occurred.
    static pResourcePack parsePack(const std::filesystem::path& resFile) noexcept(false);
    
    // Read a file by path, the file pointer and file size
    // will be stored in @p and @size. Return true if succuss.
    // the pathname should be like "./path/to/file.jpg"
    bool readResource(std::string pathname, uint8_t** p, uint32_t* size) const;
    
    // Read a file by path, the file pointer and file size
    // will be stored in @p and @size. Return true if succuss.
    // the pathname should be like "./path/to/file.jpg"
    bool readResource(std::string pathname, char** p, uint32_t* size) const {
        return readResource(pathname, (uint8_t**)p, size);
    }
   
};

typedef ResourcePack* pResourcePack;
