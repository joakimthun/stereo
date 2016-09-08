#include "core/assembly.h"

int main(int argc, char *argv[])
{
	auto assembly = stereo::Assembly::load_from_file("../test_files/ConsoleApplication2.exe");

	return 0;
}