#include "test.hpp"

int main()
{
	dav::i8080::Test t {};
	t.load_file("tests/cpudiag.bin", 0x100);
	t.run();
	return 0;
}