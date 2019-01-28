#pragma once

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

	static ResourcePack parsePack(const std::wstring& resFile) noexcept(false);
	bool readResource(const std::wstring& pathname, uint8_t** p, uint32_t* size) const;
	bool readResource(const std::wstring& pathname, char** p, uint32_t* size) const {
		return readResource(pathname, (uint8_t**)p, size);
	}

};

