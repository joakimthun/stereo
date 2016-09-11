#include "pe\pe_image_reader.h"
#include "assemblies\assembly.h"

int main(int argc, char *argv[])
{
	auto exe_reader = stereo::pe::PEImageReader::load_image("../test_files/ConsoleApplication_Any.exe");
    //auto dll_reader = stereo::pe::PEImageReader::load_image("../test_files/ClassLibrary1.dll");

    auto exe_assembly = std::make_unique<stereo::assemblies::Assembly>(exe_reader.get());
    auto ep = exe_assembly->get_entry_point();

	return 0;
}