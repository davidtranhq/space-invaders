#include "test.hpp"
#include <fstream>
#include <array>
#include <vector>
#include <iomanip>

namespace dav
{
	
namespace i8080
{
	
uint8_t rd_byte(void *mem, const uint16_t addr)
{
	return static_cast<uint8_t *>(mem)[addr];
}

void wr_byte(void *mem, const uint16_t addr, const uint8_t val)
{
	static_cast<uint8_t *>(mem)[addr] = val;
}
	
Test::Test()
	: cpu1_(), cpu2_ {nullptr}, memory_ {}
{
	cpu2_ = new ::i8080;
	::i8080_init(cpu2_);
	cpu2_->userdata = &memory_;
	cpu2_->read_byte = &rd_byte;
	cpu2_->write_byte = &wr_byte;
}

size_t Test::load_file(const std::string &path, uint16_t offset)
{
	cpu1_.load_program(path, offset);
	std::ifstream in(path, std::ios::in | std::ios::binary);
	if (in)
	{
		std::vector<uint8_t> buf;
		in.seekg(0, std::ios::end);
		size_t len {static_cast<size_t>(in.tellg())};
		buf.resize(len);
		in.seekg(0, std::ios::beg);
		in.read(reinterpret_cast<char *>(buf.data()), buf.size());
		in.close();
		for (size_t i {0}; i < buf.size(); ++i)
			cpu2_->write_byte(memory_.data(), offset+i, buf[i]);
		cpu2_->pc = offset;
		return len;
	}
	else
		return 0;
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
	for (int i = 0; i < cpu1_.memory().size(); ++i)
	{
		if (cpu1_.memory()[i] != memory_[i])
		{
			std::cerr << "Memory mismatch at " << i << "\n";
			return false;
		}
	}
	return true;
}

void Test::run()
{
	for (;;)
	{
		uint16_t pc {cpu1_.pc_};
		if (pc == 5)
		{
			// print characters stored in memory at (DE) until '$'
			if (cpu1_.c_ == 9)
			{
				uint16_t adr = cpu1_.pair(cpu1_.d_, cpu1_.e_);
				int i {0};
				do
				{
					printf("%c", cpu1_.mem_[i]);
					++i;
				} while (cpu1_.mem_[i] != '$');
			}
			// print a single char stored in register E
			if (cpu1_.c_ == 2)
				printf("%c", cpu1_.e_);
		}
		cpu1_.emulate_op();
		i8080_step(cpu2_);
		if (!compare())
		{
			std::cout << '\n';
			std::cout << std::setw(50) << std::setfill('*') << " \n";
			cpu1_.debug_info();
			i8080_debug_output(cpu2_);
			printf("Flags: %d %d %d %d %d\n",
				cpu2_->zf, cpu2_->sf, cpu2_->pf, cpu2_->cf, cpu2_->hf);
			std::terminate();
		}
		if (cpu1_.pc_ == 0)
		{
			std::cout << "Jumped to 0x0000 from " << pc << '(' << cpu1_.cycles() << ")\n";
			std::terminate();
		}
	}
}
	
}

}
