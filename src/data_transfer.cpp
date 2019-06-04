#include "cpu.hpp"

using i8080::Cpu;

void Cpu::mov(uint8_t &r1, uint8_t r2)
{
	r1 = r2;
	cycles_ += 5;
}

void Cpu::mov_r(uint8_t &r)
{
	uint16_t adr {pair(h_, l_)};
	r = mem_[adr];
	cycles_ += 7;
}

void Cpu::mov_m(uint8_t r)
{
	uint16_t adr {pair(h_, l_)};
	mem_[adr] = r;
	cycles_ += 7;
}

void Cpu::mvi(uint8_t &r, uint8_t d)
{
	r = d;
	++pc_;
	cycles_ += 7;
}

void Cpu::mvi_m(uint8_t d)
{
	uint16_t adr {pair(h_, l_)};
	mem_[adr] = d;
	++pc_;
	cycles_ += 10;
}

void Cpu::lxi(uint8_t &r1, uint8_t &r2, uint8_t l, uint8_t h)
{
	r1 = h;
	r2 = l;
	pc_ += 2;
	cycles_ += 10;
}

void Cpu::lxi(uint16_t &r, uint8_t l, uint8_t h)
{
	r = h << 8 | l;
	pc_ += 2;
	cycles_ += 10;
}

void Cpu::lda(uint8_t l, uint8_t h)
{
	uint16_t adr {pair(h, l)};
	a_ = mem_[adr];
	pc_ += 2;
	cycles_ += 13;
}

void Cpu::sta(uint8_t l, uint8_t h)
{
	uint16_t adr {pair(h, l)};
	mem_[adr] = a_;
	pc_ += 2;
	cycles_ += 13;
}

void Cpu::lhld(uint8_t l, uint8_t h)
{
	uint16_t adr {pair(h, l)};
	l_ = mem_[adr];
	h_ = mem_[adr+1];
	pc_ += 2;
	cycles_ += 16;
}

void Cpu::shld(uint8_t l, uint8_t h)
{
	uint16_t adr {pair(h, l)};
	mem_[adr] = l_;
	mem_[adr+1] = h_;
	pc_ += 2;
	cycles_ += 16;
}

void Cpu::ldax(uint8_t r1, uint8_t r2)
{
	uint16_t adr {pair(r1, r2)};
	a_ = mem_[adr];
	cycles_ += 7;
}

void Cpu::stax(uint8_t r1, uint8_t r2)
{
	uint16_t adr {pair(r1, r2)};
	mem_[adr] = a_;
	cycles_ += 7;
}

void Cpu::xchg()
{
	uint8_t tmp {h_};
	h_ = d_;
	d_ = tmp;
	tmp = l_;
	l_ = e_;
	e_ = tmp;
	cycles_ += 4;
}
