#include "cpu.hpp"

using i8080::Cpu;

void Cpu::ana(uint8_t r)
{
	set_flags(a_ & r);
	cf_.cy = 0;
	cf_.ac = ( (a_ & 0x08) | (r & 0x08) );
	// for some reason, according to 8085 manual, AC flag is set hear for ANA
	// based on the logical OR of bits 3 of the operands
	// http://www.nj7p.org/Manuals/PDFs/Intel/9800301D.pdf p.22
	a_ &= r;
	cycles_ += 4;
}

void Cpu::ana_m()
{
	uint16_t adr {pair(h_, l_)};
	ana(mem_[adr]);
	cycles_ += 3;
}

void Cpu::ani(uint8_t d)
{
	ana(d);
	cf_.cy = 0;
	cf_.ac = 0;
	pc_++;
	cycles_ += 3;
}

void Cpu::xra(uint8_t r)
{
	set_flags(a_ ^ r);
	cf_.cy = 0;
	cf_.ac = 0;
	a_ ^= r;
	cycles_ += 4;
}

void Cpu::xra_m()
{
	uint16_t adr {pair(h_, l_)};
	xra(mem_[adr]);
	cycles_ += 3;
}

void Cpu::xri(uint8_t d)
{
	xra(d);
	pc_++;
	cycles_ += 3;
}

void Cpu::ora(uint8_t r)
{
	set_flags(a_ | r);
	cf_.cy = 0;
	cf_.ac = 0;
	a_ |= r;
	cycles_ += 4;
}

void Cpu::ora_m()
{
	uint16_t adr {pair(h_, l_)};
	ora(mem_[adr]);
	cycles_ += 3;
}

void Cpu::ori(uint8_t d)
{
	ora(d);
	pc_++;
	cycles_ += 3;
}

void Cpu::cmp(uint8_t r)
{
	dif_flags(a_, r);
	cycles_ += 4;
}

void Cpu::cmp_m()
{
	uint16_t adr {pair(h_, l_)};
	dif_flags(a_, mem_[adr]);
	cycles_ += 7;
}

void Cpu::cpi(uint8_t d)
{
	cmp(d);
	++pc_;
	cycles_ += 3;
}

void Cpu::rlc()
{
	uint8_t high_bit = (a_ & 0x80) >> 7;
	a_ <<= 1;
	cf_.cy = high_bit;
	a_ |= high_bit;
	cycles_ += 4;
}

void Cpu::rrc()
{
	uint8_t low_bit = (a_ & 0x01);
	a_ >>= 1;
	cf_.cy = low_bit;
	a_ |= (low_bit << 7);
	cycles_ += 4;
}

void Cpu::ral()
{
	uint8_t high_bit = (a_ & 0x80) >> 7;
	a_ <<= 1;
	a_ |= cf_.cy;
	cf_.cy = high_bit;
	cycles_ += 4;
}

void Cpu::rar()
{
	uint8_t low_bit = a_ & 0x01;
	a_ >>= 1;
	a_ |= (cf_.cy << 7);
	cf_.cy = low_bit;
	cycles_ += 4;
}

void Cpu::cma()
{
	a_ = ~a_;
	cycles_ += 4;
}

void Cpu::cmc()
{
	cf_.cy = (cf_.cy) ? 0 : 1; 
	cycles_ += 4;
}

void Cpu::stc()
{
	cf_.cy = 1;
	cycles_ += 4;
}