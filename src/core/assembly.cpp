#include "assembly.h"

namespace stereo {

	Assembly::Assembly(u8 * raw, u64 raw_length, AssemblyType assembly_type, Platform platform) : raw_(raw), raw_length_(raw_length), assembly_type_(assembly_type), platform_(platform) {}

	Assembly::~Assembly()
	{
		delete[] raw_;
	}

	std::unique_ptr<Assembly> Assembly::load_from_file(const char * path)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			std::cout << "Could not open the specified file path" << std::endl;

			return nullptr;
		}

		auto size = file.tellg();
		auto content = new u8[size];
		file.seekg(0, std::ios::beg);
		file.read((char*)content, size);
		file.close();

		auto valid = true;

		// Validate DOS header
		if (content[0] != 0x4d || content[1] != 0x5a)
		{
			valid = false;
		}

		// Validate PE/COFF header
		auto pe_sig_offset = *(content + 0x3c);
		auto c = content + pe_sig_offset;
		if (c[0] != 0x50 || c[1] != 0x45 || c[2] != 0x00 || c[3] != 0x00)
		{
			valid = false;
		}

		// Go to assembly type spec
		c += 22;
		auto assembly_type = AssemblyType::EXE;

		if ((c[0] == 0x0 && c[1] == 0x20) || (c[0] == 0x22 && c[1] == 0x20))
		{
			assembly_type = AssemblyType::DLL;
		}

		// Go to Platform type spec
		c += 2;
		auto platform = Platform::X64;

		if (c[0] == 0xb && c[1] == 0x1)
		{
			platform = Platform::AnyCPU;
		}

		if (!valid)
		{
			delete[] content;
			std::cout << "Invalid file" << std::endl;
			return nullptr;
		}

		std::cout << "File loaded" << std::endl;

		return std::make_unique<Assembly>(content, size, assembly_type, platform);
	}

}