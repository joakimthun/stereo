#include "common/console_logger.h"
#include "pe/pe_image_reader.h"
#include "assemblies/assembly.h"
#include "exceptions/stereo_exception.h"
#include "runtime/execution_engine.h"

int main(int argc, char *argv[])
{
    auto logger = stereo::logging::ConsoleLogger();

    try
    {
        //auto assembly = std::make_unique<stereo::assemblies::Assembly>("../test_files/ConsoleApplication_Any.exe");
        auto assembly = std::make_unique<stereo::assemblies::Assembly>("../test_files/ConsoleApplication_Any_InstMethod.exe");
        //auto assembly = std::make_unique<stereo::assemblies::Assembly>("../test_files/ClassLibrary1.dll");

        auto module_def = assembly->get_module_def();
        logger.LogInfo(L"Module name -> " + module_def->name);

        auto ee = std::make_unique<stereo::runtime::ExecutionEngine>(assembly.get());
        ee->execute();
    }
    catch (const stereo::exceptions::StereoException& ex)
    {
        logger.LogError(ex.msg());
    }

	return 0;
}