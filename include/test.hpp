// runs two emulators concurrently for debugging purposes
#include "cpu.hpp"

extern "C"
{
	#include "i8080.h"
}

namespace dav
{
	
namespace i8080
{

class Test
{
	public:
	explicit Test();
	
	size_t load_file(const std::string &path, uint16_t off);
	bool compare();
	void run();
	
	private:
	Cpu cpu1_;
	::i8080 *cpu2_;
	std::array<uint8_t, 0x10000> memory_; // memory for cpu2
};
	
}

}