#include "common\console_logger.h"
#include "pe\pe_image_reader.h"
#include "assemblies\assembly.h"

int main(int argc, char *argv[])
{
    auto logger = stereo::common::ConsoleLogger();

	auto exe_reader = stereo::pe::PEImageReader::load_image("../test_files/ConsoleApplication_Any.exe");
    //auto dll_reader = stereo::pe::PEImageReader::load_image("../test_files/ClassLibrary1.dll");

    auto exe_assembly = std::make_unique<stereo::assemblies::Assembly>(exe_reader.get());
    auto ep = exe_assembly->get_entry_point();

    exe_assembly->read_module();
    exe_assembly->read_method(ep);

    logger.LogInfo("Module name -> " + exe_assembly->get_module()->name);
    logger.LogInfo("Entry point name -> " + exe_assembly->get_methods()[0]->name);

	return 0;
}