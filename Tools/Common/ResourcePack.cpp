#include "pch.h"
#include "ResourcePack.h"
#include <fstream>
#include <sstream>

inline void _trace(std::wstring str) {
#ifdef _DEBUG
	OutputDebugString((str + L'\n').c_str());
#endif
}

using namespace std::filesystem;

bool generatePack(const std::wstring& destFile, const path& resDirectory)
{
	// destination
	std::ofstream dst(destFile, std::ios::binary);

	std::vector<uint32_t> folder_size = { 0 };
	std::vector<path> folder_path = { resDirectory };
	std::vector<directory_iterator> _i;		// used to store i of parent folder
	std::vector<std::string> _foldername;	// used to store foldername of parent folder
	directory_iterator i;
	uint8_t depth = 0;

recursive:
	// enter the directory
	std::string foldername = folder_path.back().filename().u8string();

	// write entity + file name
	entity e{ TYPE_FOLDER, 0, (uint16_t)foldername.size(), 0 };
	dst.write((char*)&e, sizeof entity);
	dst << foldername;

	// walk the directory
	for (i = directory_iterator(folder_path.back());
		i != directory_iterator();
		++i)
	{
		if (depth == 255)
			return false;		// not support depth > 255

		if (i->is_directory())
		{
			_trace(static_cast<path>(*i).filename());

			folder_path.push_back(*i);
			folder_size.push_back(0);

			// back up current i & folder name
			_i.push_back(i);
			_foldername.push_back(foldername);
			++depth;

			goto recursive;
		resume:
			// restore backed i & folder name
			i = _i.back();
			foldername = _foldername.back();
			_i.pop_back();
			_foldername.pop_back();
		}
		else if (i->is_regular_file())
		{
			_trace(static_cast<path>(*i).filename());
			
			// get utf-8 encoded file name
			std::string filename = static_cast<path>(*i).filename().u8string();
			uint32_t file_size = (uint32_t)i->file_size();
			uint16_t name_size = (uint16_t)filename.size();

			// write entity + file name + data
			entity e{ TYPE_FILE, 0, name_size, file_size };
			dst.write((char*)&e, sizeof(entity));
			dst << filename;
			dst << std::ifstream(*i, std::ios::binary).rdbuf();

			// update folder size
			folder_size.back() += sizeof(entity) + name_size + file_size;
		}
	}
	// set directory size
	int _s = folder_size.back();
	dst.seekp(-_s - (int)foldername.size() - (int)sizeof(uint32_t), std::ios::cur);
	dst.write((char*)&_s, sizeof(uint32_t));
	dst.seekp(0, std::ios::end);
	// back to parent directory
	folder_size.pop_back();
	folder_path.pop_back();

	if (!folder_size.empty()) {
		folder_size.back() += (int)sizeof(entity) + (int)foldername.size() + _s;
		--depth;
		goto resume;
	}
	return true;
}

void parsePack(std::istream& res,
	std::function<void (const path&)> new_folder,
	std::function<void (std::istream&, const path&, uint32_t)> new_file)
{
	path relativePath(".");
	std::vector<uint32_t> folder_size;

	do {
		entity e;
		res.read((char*)&e, sizeof(entity));

		// read name
		char* name = new char[e.nameSize + 1];
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

#if _DEBUG
			std::streampos p = res.tellg();

			// dump file or read file
			new_file(res, relativePath, e.dataSize);

			// you must consume these bytes
			_ASSERT(res.tellg() == p + std::streamoff(e.dataSize));
#else
			// dump file or read file
			new_file(res, relativePath, e.dataSize);
#endif

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

bool extractPack(const path& resFile)
{
	std::ifstream res(resFile, std::ios::binary);

	std::ostringstream extract_dir_name;
	extract_dir_name << "extract_" << time(NULL);
	path folder_path = resFile.parent_path() / extract_dir_name.str();

	create_directory(folder_path);	// extract files to this folder

	try {
		parsePack(
			res,
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

ResourcePack ResourcePack::parsePack(const path& resFile)
{
	std::ifstream res(resFile, std::ios::binary);
	ResourcePack pack;
	::parsePack(
		res,
		// new folder
		[](const path& relativePath) { },
		// new file: read into RAM
		[&pack](std::istream& res, const path& relativePath, uint32_t dataSize) {
			uint8_t* data = new uint8_t[dataSize];
			res.read((char*)data, dataSize);
			pack.list.emplace_back(relativePath, data, dataSize);
		}
	);

	return pack;
}

bool ResourcePack::readResource(const std::wstring& pathname, uint8_t** p, uint32_t* size) const
{
	// the pathname should be like "./path/to/file.jpg"
	if (pathname[0] != L'.')
		return false;

	// Converts all directory separators in the generic-format view of the
	// path to the preferred directory separator. 
	const std::wstring pathname_ = static_cast<path>(pathname).make_preferred().wstring();

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
