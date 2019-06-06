// runs two emulators concurrently for debugging purposes

namespace superzazu
{
	extern "C"
	{
		#include "i8080.h"
	}
}

namespace i8080
{

class Test
{
	public:
	explicit Test();
	
	private:
	Cpu cpu1_;
	superzazu::i8080 cpu2_;
	std::array<uint8_t, 0x10000> memory_; // memory for cpu2
}
	
}