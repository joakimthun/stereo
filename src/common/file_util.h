#pragma once

#include <fstream>
#include <memory>

#include "typedef.h"

namespace stereo {

	struct FileContent
	{
		FileContent() : content(nullptr), size(0) {}
		FileContent(u8* content, u64 size) : content(content), size(size) {}
		~FileContent() { delete[] content; }

		u8* content;
		u64 size;
	};

	inline std::unique_ptr<FileContent> load_entire_file(const char* path) 
	{
		std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			return nullptr;
		}

		auto size = file.tellg();
		auto content = new u8[size];
		file.seekg(0, std::ios::beg);

		file.read((char*)content, size);
		file.close();

		return std::make_unique<FileContent>(content, size);
	}

}
