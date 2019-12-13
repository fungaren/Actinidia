#pragma once

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

struct entity {
    uint8_t type;       // entity type
    uint8_t reserved;   // not used, maybe attribute
    uint16_t nameSize;  // do not support filename length > 65535
    //uint32_t checksum;
    uint32_t dataSize;  // do not contains sizeof(entity) and name
                        // do not support filesize > 4GB
};

bool generatePack(const std::wstring& destFile, const std::filesystem::path& resDirectory);
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
    struct ResourceFile {
        std::wstring path;
        uint8_t* data;
        uint32_t dataSize;
        ResourceFile(std::wstring path, uint8_t* data, uint32_t dataSize) :
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
    ResourcePack(ResourcePack&) = delete;
    ResourcePack(ResourcePack&& r) : list(r.list) {};

public:

    static ResourcePack parsePack(const std::filesystem::path& resFile) noexcept(false);
    bool readResource(const std::wstring& pathname, uint8_t** p, uint32_t* size) const;
    bool readResource(const std::wstring& pathname, char** p, uint32_t* size) const {
        return readResource(pathname, (uint8_t**)p, size);
    }

};

