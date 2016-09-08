#include "core/assembly.h"

int main(int argc, char *argv[])
{
	auto exe = stereo::Assembly::load_from_file("../test_files/ConsoleApplication_Any.exe");

	auto dll = stereo::Assembly::load_from_file("../test_files/ClassLibrary1.dll");

	return 0;
}