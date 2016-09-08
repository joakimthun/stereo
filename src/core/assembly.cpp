#include "assembly.h"

namespace stereo {

	Assembly::Assembly(byte * raw, u64 raw_length) : raw_(raw), raw_length_(raw_length) {}

	Assembly::~Assembly()
	{
		delete[] raw_;
	}

	std::unique_ptr<Assembly> Assembly::load_from_file(const char * path)
	{
		std::streampos size;
		byte * content;

		std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			std::cout << "Unable to open file";

			return nullptr;
		}

		size = file.tellg();
		content = new byte[size];
		file.seekg(0, std::ios::beg);
		file.read(content, size);
		file.close();

		std::cout << "the entire file content is in memory";

		if (content[0] != 0x4d || content[1] != 0x5a)
		{
			delete[] content;
			std::cout << "invalid dos header";
			return nullptr;
		}

		return std::make_unique<Assembly>(content, size);
	}

}