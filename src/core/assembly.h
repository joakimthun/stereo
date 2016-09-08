#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#include "../common/typedef.h"

namespace stereo {

	class Assembly
	{
	public:
		Assembly(byte * raw, u64 raw_length);
		~Assembly();
		static std::unique_ptr<Assembly> load_from_file(const char * path);

	private:
		byte * raw_;
		u64 raw_length_;
	};

}