#include "cpu.hpp"

using i8080::Cpu;

// arithmetic group


bool parity(uint16_t x)
{
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (~x) & 1;
}

void Cpu::set_flags(uint16_t res)
{
	cf_.z = (res == 0);
	cf_.s = ((res & 0x80) == 0x80);
	cf_.p = (parity(res));
	cf_.cy = (res > 0xFF);
	cf_.ac = (res > 0x0F);
}

void Cpu::add(uint8_t r)
{
	uint16_t sum = static_cast<uint16_t>(a_) + static_cast<uint16_t>(r);
	set_flags(sum);
	a_ = a_ + r;
	cycles_ += 4;
}

void Cpu::add_m()
{
	add(mem_[pair(h_, l_)]);
	cycles_ += 3;
}


void Cpu::adi(uint8_t d)
{
	add(d);
	++pc_;
	cycles_ += 3;
}

void Cpu::adc(uint8_t r)
{
	uint16_t sum = static_cast<uint16_t>(a_) + static_cast<uint16_t>(r)
		+ static_cast<uint16_t>(cf_.cy);
	set_flags(sum);
	a_ = r + cf_.cy;
	cycles_ += 4;
}

void Cpu::adc_m()
{
	adc(mem_[pair(h_, l_)]);
	cycles_ += 3;
}

void Cpu::aci(uint8_t d)
{
	adc(d);
	++pc_;
	cycles_ += 3;
}

void Cpu::sub(uint8_t r)
{
	uint16_t dif = static_cast<uint16_t>(a_) - static_cast<uint16_t>(r);
	set_flags(dif);
	a_ -= r;
	cycles_ += 4;
}

void Cpu::sub_m()
{
	sub(mem_[pair(h_, l_)]);
	cycles_ += 3;
}

void Cpu::sui(uint8_t d)
{
	sub(d);
	++pc_;
	cycles_ += 3;
}

void Cpu::sbb(uint8_t r)
{
	uint16_t dif = static_cast<uint16_t>(a_) - static_cast<uint16_t>(r)
		- static_cast<uint16_t>(cf_.cy);
	set_flags(dif);
	a_ = a_ - r - cf_.cy;
	cycles_ += 4;
}

void Cpu::sbb_m()
{
	sbb(mem_[pair(h_, l_)]);
	cycles_ += 3;
}

void Cpu::sbi(uint8_t d)
{
	sbb(d);
	++pc_;
	cycles_ += 3;
}

void Cpu::inr(uint8_t &r)
{
	uint8_t cy_flag {cf_.cy};
	uint16_t sum = static_cast<uint16_t>(r) + 1;
	set_flags(sum);
	cf_.cy = cy_flag; // INR doesn't affect the cy flag
	++r;
	cycles_ += 5;
}

void Cpu::inr_m()
{
	inr(mem_[pair(h_, l_)]);
	cycles_ += 5;
}

void Cpu::dcr(uint8_t &r)
{
	uint8_t cy_flag {cf_.cy};
	uint16_t dif = static_cast<uint16_t>(r) - 1;
	set_flags(dif);
	cf_.cy = cy_flag; // DCR doesn't affect the cy flag
	--r;
	cycles_ += 5;
}

void Cpu::dcr_m()
{
	dcr(mem_[pair(h_, l_)]);
	cycles_ += 5;
}

void Cpu::inx(uint8_t &r1, uint8_t &r2)
{
	++r2;
	if (r2 == 0)
		++r1;
	cycles_ += 5;
}

void Cpu::inx(uint16_t &r)
{
	++r;
	++cycles_;
	cycles_ += 5;
}

void Cpu::dcx(uint8_t &r1, uint8_t &r2)
{
	if (r1 == 0)
	{
		// underflow of high 8-bits
		if (r2 == 0)
		{
			// underflow of low 8-bits
			--r1;
			--r2;
		}
		else
			--r2;
	}
	else
		--r1;
	cycles_ += 5;
}

void Cpu::dcx(uint16_t &r)
{
	--r;
	++cycles_;
}

void Cpu::dad(uint8_t &r1, uint8_t &r2)
{
	uint32_t sum = static_cast<uint32_t>(pair(h_, l_))
		+ static_cast<uint32_t>(pair(r1, r2));
	cf_.cy = (sum > 0xFFFF);
	uint16_t res = pair(h_, l_) + pair(r1, r2);
	l_ = static_cast<uint8_t>(res & 0xFF);
	h_ = static_cast<uint8_t>((res & 0xFF00) >> 8);
	cycles_ += 5;
}

void Cpu::dad(uint16_t &r)
{
	uint32_t sum = static_cast<uint32_t>(r) + static_cast<uint32_t>(pair(h_, l_));
	cf_.cy = (sum > 0xFFFF);
	uint16_t res = pair(h_, l_) + r;
	r = res;
	cycles_ += 10;
}

void Cpu::daa()
{
	int old_cycles {cycles_};
	// if the value of the least significant 4 bits of the accumulator is
	// greater than 9 or if the AC flag is set, 6 is added to the accumulator
	if (((a_ & 0x0F) > 9) || cf_.ac)
		adi(6);
	// if the value of the most significant 4 bits of the accumulator is now
	// greater than 9 or if the AC flag is set, 6 is added to the most
	// significant 4 bits of the accumulator
	if ((((a_ & 0xF0) >> 4) > 9) || cf_.ac)
		adi(0x60);
	cycles_ = old_cycles + 4;
}