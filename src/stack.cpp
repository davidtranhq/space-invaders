#include "cpu.hpp"

using i8080::Cpu;

void Cpu::push(uint8_t r1, uint8_t r2)
{
	mem_[sp_ - 1] = r1;
	mem_[sp_ - 2] = r2;
	sp_ -= 2;
	cycles_ += 11;
}

void Cpu::push_psw()
{
	mem_[sp_ - 1] =  a_;
	// flag word : S-Z-0-AC-0-P-1-CY
	uint8_t flags {0};
	flags |= cf_.cy;
	flags |= 0x01 << 1;
	flags |= cf_.p << 2;
	flags |= 0x00 << 3;
	flags |= cf_.ac << 4;
	flags |= 0x00 << 5;
	flags |= cf_.z << 6;
	flags |= cf_.s << 7;
	mem_[sp_ - 2] = flags;
	sp_ -= 2;
	cycles_ += 11;
}

void Cpu::pop(uint8_t &r1, uint8_t &r2)
{
	r1 = mem_[sp_ + 1];
	r2 = mem_[sp_];
	sp_ += 2;
	cycles_ += 10;
}

void Cpu::pop_psw()
{
	uint8_t word {mem_[sp_]};
	cf_.cy = word & 0x01;
	cf_.p = (word & 0x03) >> 2;
	cf_.ac = (word & 0x05) >> 4;
	cf_.z = (word & 0x07) >> 6;
	cf_.s = (word & 0x08) >> 7;
	a_ = mem_[sp_ + 1];
	sp_ += 2;
	cycles_ += 10;
}

void Cpu::xthl()
{
	uint8_t tmp {l_};
	l_ = mem_[sp_];
	mem_[sp_] = tmp;
	
	tmp = h_;
	h_ = mem_[sp_ + 1];
	mem_[sp_ + 1] = h_;
	cycles_ += 18;
}

void Cpu::sphl()
{
	uint16_t d16 {pair(h_, l_)};
	sp_ = d16;
	cycles_ += 5;
}

void Cpu::in(uint8_t port)
{
	a_ = in_handle_(port);
	++pc_;
	cycles_ += 10;
}

void Cpu::out(uint8_t port)
{
	out_handle_(port, a_);
	++pc_;
	cycles_ += 10;
}

void Cpu::ei()
{
	int_enabled_ = true;
	cycles_ += 4;
}

void Cpu::di()
{
	int_enabled_ = false;
	cycles_ += 4;
}

void Cpu::hlt()
{
	halted_ = true;
	cycles_ += 7;
}

void Cpu::nop()
{
	cycles_ += 4;
}

