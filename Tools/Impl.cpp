#include "pch.h"

inline void _trace(std::wstring str) {
#ifdef _DEBUG
	OutputDebugString((str + L'\n').c_str());
#endif
}

using namespace std::filesystem;

const uint8_t TYPE_FOLDER = 0x01;
const uint8_t TYPE_FILE = 0x00;

struct entity {
	uint8_t type;		// entity type
	uint8_t reserved;	// not used, maybe attribute
	uint16_t nameSize;	// do not support filename length > 65535
	//uint32_t checksum;
	uint32_t dataSize;	// do not contains sizeof(entity) and name
						// do not support filesize > 4GB
};

bool generatePack(const std::wstring& destFile, const path& resDirectory)
{
	// destination
	std::ofstream dst(destFile, std::ios::binary);

	std::vector<uint32_t> folder_size = { 0 };
	std::vector<path> folder_path = { resDirectory };
	std::vector<directory_iterator> _i;
	std::vector<std::string> _foldername;
	directory_iterator i;
	uint8_t depth = 0;

recursive:
	std::string foldername = folder_path.back().filename().u8string();
	entity e{ TYPE_FOLDER, 0, (uint16_t)foldername.size(), 0 };
	dst.write((char*)&e, sizeof entity);
	dst << foldername;
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
			_i.push_back(i);
			_foldername.push_back(foldername);
			++depth;
			goto recursive;
		resume:
			i = _i.back();
			foldername = _foldername.back();
			_i.pop_back();
			_foldername.pop_back();
		}
		else if (i->is_regular_file())
		{
			_trace(static_cast<path>(*i).filename());
			std::string filename = static_cast<path>(*i).filename().u8string();
			uint32_t file_size = i->file_size();
			uint16_t name_size = (uint16_t)filename.size();
			entity e{ TYPE_FILE, 0, name_size, file_size };
			dst.write((char*)&e, sizeof(entity));
			dst << filename;
			dst << std::ifstream(*i, std::ios::binary).rdbuf();
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

bool extractPack(const path& resFile)
{
	std::ifstream res(resFile, std::ios::binary);
	std::ostringstream extract_dir_name;
	extract_dir_name << "extract_" << time(NULL);
	path folder_path = resFile.parent_path() / extract_dir_name.str();
	create_directory(folder_path);
	path _path = folder_path;

	std::vector<uint32_t> folder_size;

	do {
		entity e;
		res.read((char*)&e, sizeof(entity));

		// read name
		char* name = new char[e.nameSize + 1];
		res.read(name, e.nameSize);
		name[e.nameSize] = '\0';
		// get absolute path
		try {
			_path /= u8path(name);
		} catch (std::exception e) {
			delete[] name;
			return false;
		}

		if (e.type == TYPE_FOLDER)
		{
			create_directory(_path);

			if (e.dataSize == 0) {
				// empty folder
				folder_size.back() -= e.nameSize + sizeof(entity);
				_path =_path.parent_path();
			} else {
				// not empty folder
				if (!folder_size.empty())
					folder_size.back() -= e.nameSize + sizeof(entity) + e.dataSize;
				folder_size.push_back(e.dataSize);
			}
		}
		else if (e.type == TYPE_FILE)
		{
			folder_size.back() -= e.dataSize + e.nameSize + sizeof(entity);

			std::ofstream out(_path, std::ios::binary);
			
			char buffer[4096];
			while (e.dataSize > 4096) {
				res.read(buffer, 4096);
				out.write(buffer, 4096);
				e.dataSize -= 4096;
			}
			res.read(buffer, e.dataSize);
			out.write(buffer, e.dataSize);

			_path = _path.parent_path();
		}

		delete[] name;

		while (!folder_size.empty() && folder_size.back() == 0) {
			// no more files/folders to load
			folder_size.pop_back();
			_path = _path.parent_path();
		}
	} while (_path != folder_path);
	return true;
}

#include "Common/Canvas.h"
bool imageConcatenate(const std::wstring& destImage, const std::vector<std::wstring>& imgFiles)
{	 
	std::vector<ImageMatrix> imgs;
	uint32_t width = 0, total_height = 0;
	try {
		for (auto& file : imgFiles) {
			std::wstring extName = file.substr(file.rfind(L'.') + 1);
			if (extName == L"png" || extName == L"PNG")
				imgs.emplace_back(ImageMatrixFactory::fromPngFile(file.c_str()));
			else
				imgs.emplace_back(ImageMatrixFactory::fromJpegFile(file.c_str()));
			if (width == 0)
				width = imgs.back().getWidth();
			else if (imgs.back().getWidth() != width)
				throw std::runtime_error("Images width don't match.\n"\
					"Only images with the same width can be concatenated.");
			total_height += imgs.back().getHeight();
			if (total_height > std::numeric_limits<uint16_t>::max())
				throw std::runtime_error("Image height is too large. \n"\
					"Reduce the number of images and try again.");
		}
	}
	catch (std::runtime_error e) {
		MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	ImageMatrix temp = ImageMatrixFactory::createBufferImage(width, total_height, Canvas::Constant::alpha);
	uint16_t y = 0;
	for (const ImageMatrix& i : imgs) {
		PiCanvas::blend(temp, i, 0, y, 0xFF);
		y += i.getHeight();
	}
	try {
		std::wstring extName = destImage.substr(destImage.rfind(L'.') + 1);
		if (extName == L"png" || extName == L"PNG")
			ImageMatrixFactory::dumpPngFile(temp, destImage.c_str());
		else
			ImageMatrixFactory::dumpJpegFile(temp, destImage.c_str(), 100);
	}
	catch (std::runtime_error e) {
		MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}
