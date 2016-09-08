#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#include "../common/typedef.h"

namespace stereo {

	enum class AssemblyType : u8 
	{
		EXE = 0,
		DLL = 1
	};

	enum class Platform : u8
	{
		AnyCPU = 0,
		X64 = 1
	};

	class Assembly
	{
	public:
		Assembly(u8 * raw, u64 raw_length, AssemblyType assembly_type, Platform platform);
		~Assembly();
		static std::unique_ptr<Assembly> load_from_file(const char * path);

	private:
		u8 * raw_;
		u64 raw_length_;
		AssemblyType assembly_type_;
		Platform platform_;
	};

}