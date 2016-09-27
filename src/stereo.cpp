#include "common\console_logger.h"
#include "pe\pe_image_reader.h"
#include "assemblies\assembly.h"
#include "exceptions\stereo_exception.h"

int main(int argc, char *argv[])
{
    auto logger = stereo::logging::ConsoleLogger();

    try
    {
        auto exe_assembly = std::make_unique<stereo::assemblies::Assembly>("../test_files/ConsoleApplication_Any.exe");
        //auto dll_assembly = std::make_unique<stereo::assemblies::Assembly>("../test_files/ClassLibrary1.dll");

        auto ep = exe_assembly->get_entry_point();

        auto module_def = exe_assembly->get_module_def();
        auto method_def = exe_assembly->get_method_def(ep);

        logger.LogInfo(L"Module name -> " + module_def->name);
        logger.LogInfo(L"Entry point name -> " + method_def->name);

        //throw stereo::exceptions::StereoException("StereoException thrown!");
    }
    catch (const stereo::exceptions::StereoException& ex)
    {
        logger.LogError(ex.what());
    }

	return 0;
}