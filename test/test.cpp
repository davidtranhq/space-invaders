#include "cpu.hpp"
extern "C"
{
	#include "i8080cpu.h"
}
#include <cstdio>

namespace i8080
{
	class Test;
}

class i8080::Test
{
	public:
	Test(const std::string &f);
	
	void execute(bool success_check);
	bool compare();
	
	private:
	i8080::Cpu cpu1_;
	i8080cpu *cpu2_;
};

i8080::Test::Test(const std::string &f)
	: cpu1_ {}, cpu2_ {nullptr}
{
	cpu1_.load_program(f, 0x100);  
	cpu1_.mem_[5] = 0xC9; // inject RET at 0x0005 to handle CP/M "CALL 5"
	
	i8080cpu_init(&cpu2_);
	i8080cpu_load(cpu2_, f.c_str(), 0x100);
}

bool i8080::Test::compare()
{
	if (!(cpu1_.a_ == cpu2_->a
		&& cpu1_.b_ == cpu2_->b
		&& cpu1_.c_ == cpu2_->c
		&& cpu1_.d_ == cpu2_->d
		&& cpu1_.e_ == cpu2_->e
		&& cpu1_.h_ == cpu2_->h
		&& cpu1_.l_ == cpu2_->l))
	{	
		std::cerr << "Register mismatch!\n";
		return false;
	}
	if (!(cpu1_.cf_.s == cpu2_->sf
		&& cpu1_.cf_.z == cpu2_->zf
		&& cpu1_.cf_.ac == cpu2_->hf
		&& cpu1_.cf_.p == cpu2_->pf
		&& cpu1_.cf_.cy == cpu2_->cf))
	{
		std::cerr << "Flag mismatch!\n";
		return false;
	}
	if (cpu1_.cycles_ != cpu2_->cyc)
	{
		std::cerr << "Cycle mismatch!\n";
		return false;
	}
	if (cpu1_.pc_ != cpu2_->pc)
	{
		std::cerr << "Program counter mismatch!\n";
		return false;
	}
	if (cpu1_.sp_ != cpu2_->sp)
	{
		std::cerr << "Stack pointer mismatch!\n";
		return false;
	}
	return true;
}

void i8080::Test::execute(bool success_check)
{
	bool success {false};
	for (;;)
	{
		int const pc {cpu1_.pc_};
		if (cpu1_.mem_[pc] == 0x76)
		{
			std::cout << "HLT at 0x" << std::hex << pc << '\n';
			std::terminate();
		}
		if (pc == 0x0005)
		{
			if (cpu1_.c_ == 9)
			{
				int i;
				int de {static_cast<uint16_t>(cpu1_.d_) << 8 | cpu1_.e_};
				for (i = de; cpu1_.mem_[i] != '$'; ++i)
					std::putchar(cpu1_.mem_[i]);
				success = true;
			}
			if (cpu1_.c_ == 2)
				putchar(static_cast<char>(cpu1_.e_));
		}
		cpu1_.emulate_op();
		i8080cpu_step(cpu2_);
		std::cout << '\n';
		cpu1_.debug_info();
		i8080cpu_debug_output(cpu2_);
		if (!compare())
		{
			std::terminate();
		}
		if (cpu1_.pc_ == 0)
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
	i8080::Test t {"cpudiag.bin"};
	t.execute(0);
	// t.execute("TEST.COM", 0);
	// t.execute("8080PRE.COM", 1);
	// t.execute("8080EX1.COM", 0);
	return 0;
}