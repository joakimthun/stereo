#include "pe\pe_image_reader.h"

int main(int argc, char *argv[])
{
	auto exe_reader = stereo::pe::PEImageReader::load_image("../test_files/ConsoleApplication_Any.exe");
    //auto dll_reader = stereo::pe::PEImageReader::load_image("../test_files/ClassLibrary1.dll");

	return 0;
}