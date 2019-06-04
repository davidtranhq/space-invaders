#include "cpu.hpp"
#include <cstdio>

namespace i8080
{
	class Test;
}

class i8080::Test
{
	public:
	void execute(const std::string &f, bool success_check);
};


void i8080::Test::execute(const std::string &f, bool success_check)
{
	bool success {false};
	i8080::Cpu cpu {};
	cpu.load_program(f, 0x100);  
	cpu.mem_[5] = 0xC9; // inject RET at 0x0005 to handle CP/M "CALL 5"
	for (;;)
	{
		int const pc {cpu.pc_};
		if (cpu.mem_[pc] == 0x76)
		{
			std::cout << "HLT at 0x" << std::hex << pc << '\n';
			std::terminate();
		}
		if (pc == 0x0005)
		{
			if (cpu.c_ == 9)
			{
				int i;
				int de {static_cast<uint16_t>(cpu.d_) << 8 | cpu.e_};
				for (i = de; cpu.mem_[i] != '$'; ++i)
					std::putchar(cpu.mem_[i]);
				success = true;
			}
			if (cpu.c_ == 2)
				putchar(static_cast<char>(cpu.e_));
		}
		cpu.emulate_op();
		if (cpu.pc_ == 0)
		{
			std::cout << "JMP to 0000 from 0x" << std::hex << pc << '\n';
			if (success_check && !success)
				std::terminate();
			return;
		}
	} 
}



int main()
{
	i8080::Test t {};
	t.execute("cpudiag.bin", 0);
	// t.execute("TEST.COM", 0);
	// t.execute("8080PRE.COM", 1);
	// t.execute("8080EX1.COM", 0);
	return 0;
}