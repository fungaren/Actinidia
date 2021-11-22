/*
 * Copyright (c) 2021, FANG All rights reserved.
 */
#pragma once
#include <optional>
#include <fstream>
#include <list>
#include <functional>

#include "Base.h"

#ifndef _ASSERT
    #define _ASSERT(...)
#endif /* _ASSERT */

const static inline uint8_t TYPE_FOLDER = 0x01;
const static inline uint8_t TYPE_FILE = 0x00;
const static inline char MAGIC[] = "ACTINIDIA";

struct entity {
    uint8_t type;       //!< entity type
    uint8_t reserved;   //!< not used, maybe attribute
    uint16_t nameSize;  //!< do not support filename length > 65535
    //uint32_t checksum;
    uint32_t dataSize;  //!< do not contains sizeof(entity) and name
                        //!< do not support filesize > 4GB
};

/**
 * @brief Resource package format:
 *  ```
    - entity header + folder name
        - entity header + file name + data
        - entity header + folder name
            - entity header + file name + data
            - entity header + file name + data
            ...
        ...
    ...
    ```
*/
struct walk_layer {
    entity e;           /**!< Entity header */
    std::string u8name;   /**!< Folder name or file name, UTF-8 encoded */
    std::optional<string_t> path; /**!< Absolute file path */
};

/**
 * @brief Walk a directory
 * @param dir Directory path
 * @return A `walk_layer` list.
 */
std::list<walk_layer> walkFolder(const string_t& dir);

/**
 * @brief Generate a resource file
 * @param destFile Output file path
 * @param tree A `walk_layer` list.
 * @param update_progress A callback to update progress
 * @return true if succeed, otherwise return false
 */
bool generatePack(
    const string_t& destFile,
    const std::list<walk_layer>& tree,
    std::function<void(size_t)> update_progress);

/**
 * @brief Extract all files from a resource file
 * @param resFile Resource file path
 * @param dir Output directory
 * @return true if succeed, otherwise return false
 */
bool extractPack(const string_t& resFile, const string_t& dir);

/**
 * @brief Resource file in memory:
 * ```
    - /filename
    - /foldername/filename
    - /foldername/filename
    - /filename
    - /foldername/foldername/filename
    - /filename
    - /filename
    ```
*/

class ResourcePack {
    struct ResourceFile {
        string_t path;
        uint8_t* data;
        uint32_t dataSize;
        ResourceFile(const string_t& path, uint8_t* data, uint32_t dataSize) :
            path(path), data(data), dataSize(dataSize)
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

    /**
     * @brief Open a package and construct a ResourcePack instance.
     * @param pathname Resource file path
     * @return pResourcePack. Will throw an exception if any error occurred.
     */
    static pResourcePack parsePack(const string_t& pathname) noexcept(false);

    /**
     * @brief Read a file by path
     * @param pathname eg. `./path/to/file.jpg`
     * @param p file data
     * @param size file size
     * @return true if succuss, otherwise return false
     */
    bool readResource(const string_t& pathname, uint8_t** p, uint32_t* size) const;

    /**
     * @brief Read a file by path
     * @param pathname eg. `./path/to/file.jpg`
     * @param p file data
     * @param size file size
     * @return true if succuss, otherwise return false
     */
    bool readResource(const string_t& pathname, char** p, uint32_t* size) const {
        return readResource(pathname, (uint8_t**)p, size);
    }
};

typedef ResourcePack* pResourcePack;
