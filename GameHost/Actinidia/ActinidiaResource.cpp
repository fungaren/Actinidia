#include "pch.h"
#include "ActinidiaResource.h"
//
//void parsePack(std::istream& res,
//	std::function<void(const path&)> new_folder,
//	std::function<void(std::istream&, const path&, uint32_t)> new_file)
//{
//	path relativePath(".");
//	std::vector<uint32_t> folder_size;
//
//	do {
//		entity e;
//		res.read((char*)&e, sizeof(entity));
//
//		// read name
//		char* name = new char[e.nameSize + 1];
//		res.read(name, e.nameSize);
//		name[e.nameSize] = '\0';
//		try {
//			relativePath /= u8path(name);
//		}
//		catch (std::exception e) {
//			delete[] name;
//			throw e;
//		}
//		delete[] name;
//
//		if (e.type == TYPE_FOLDER)
//		{
//			new_folder(relativePath);
//
//			// empty folder
//			if (e.dataSize == 0) {
//
//				// calc remaining bytes in this folder to be read
//				if (!folder_size.empty())
//					folder_size.back() -= e.nameSize + sizeof(entity);
//
//				// back to parent folder
//				relativePath = relativePath.parent_path();
//			}
//			else { // not empty folder
//
//				// calc remaining bytes in this folder to be read
//				if (!folder_size.empty())
//					folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;
//
//				// enter subfolder
//				folder_size.push_back(e.dataSize);
//			}
//		}
//		else if (e.type == TYPE_FILE)
//		{
//			// calc remaining bytes in this folder to be read
//			folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;
//
//#if _DEBUG
//			std::streampos p = res.tellg();
//
//			// dump file or read file
//			new_file(res, relativePath, e.dataSize);
//
//			// you must consume these bytes
//			_ASSERT(res.tellg() == p + std::streamoff(e.dataSize));
//#else
//			// dump file or read file
//			new_file(res, relativePath, e.dataSize);
//#endif
//
//			// back to parent folder
//			relativePath = relativePath.parent_path();
//		}
//
//		while (!folder_size.empty() && folder_size.back() == 0) {
//			// no more files/folders to load
//			folder_size.pop_back();
//			relativePath = relativePath.parent_path();
//		}
//	} while (relativePath != ".");
//}
//
//ResourcePack ResourcePack::parsePack(const std::wstring& resFile)
//{
//	std::ifstream res(resFile, std::ios::binary);
//	ResourcePack pack;
//	::parsePack(
//		res,
//		// new folder
//		[](const path& relativePath) {},
//		// new file: read into RAM
//		[&pack](std::istream& res, const path& relativePath, uint32_t dataSize) {
//		uint8_t* data = new uint8_t[dataSize];
//		res.read((char*)data, dataSize);
//		pack.list.emplace_back(relativePath, data, dataSize);
//	}
//	);
//
//	return pack;
//}
//
//bool ResourcePack::readResource(const std::wstring& pathname, uint8_t** p, uint32_t* size) const
//{
//	// the pathname should be like "./path/to/file.jpg"
//	if (pathname[0] != L'.')
//		return false;
//
//	// Converts all directory separators in the generic-format view of the
//	// path to the preferred directory separator. 
//	const std::wstring pathname_ = static_cast<path>(pathname).make_preferred().wstring();
//
//	auto i = std::find_if(list.begin(), list.end(),
//		[&pathname_](const ResourceFile& i) {
//		return (i.path == pathname_);
//	}
//	);
//
//	if (i != list.end()) {
//		// found target file, set the data pointer & size
//		*p = i->data;
//		*size = i->dataSize;
//		return true;
//	}
//	else return false;
//}
