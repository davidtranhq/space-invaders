#include "cpu.hpp"

using i8080::Cpu;

// arithmetic group

std::array<bool, 256> parity =
{
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

void Cpu::set_flags(uint8_t res)
{
	// for logical operators
	cf_.z = (res == 0);
	cf_.s = ((res & 0x80) == 0x80);
	cf_.p = (parity[res]);
}

void Cpu::sum_flags(uint8_t a, uint8_t b, uint8_t cy)
{
	uint16_t sum = static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + cy;
	cf_.z = (static_cast<uint8_t>(a + b + cy) == 0);
	cf_.s = ((sum & 0x80) == 0x80);
	cf_.p = (parity[static_cast<uint8_t>(a + b + cy)]);
	cf_.cy = (sum > 0xFF);
	a &= 0x0F;
	b &= 0x0F;
	cf_.ac = ((a + b + cy) > 0x0F);
}

void Cpu::dif_flags(uint8_t a, uint8_t b, uint8_t cy)
{
	uint16_t dif = static_cast<uint16_t>(a) - static_cast<uint16_t>(b) - cy;
	cf_.z = (dif == 0);
	cf_.s = ((dif & 0x80) == 0x80);
	cf_.p = (parity[static_cast<uint8_t>(a - b - cy)]);
	cf_.cy = (a < (b + cy));
	a &= 0x0F;
	b &= 0x0F;
	cf_.ac = (a < (b + cy));
}



void Cpu::add(uint8_t r)
{
	sum_flags(a_, r);
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
	uint8_t carry = cf_.cy;
	sum_flags(a_, r, cf_.cy);
	a_ = a_ + r + carry;
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
	dif_flags(a_, r);
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
	uint8_t carry = cf_.cy;
	dif_flags(a_, r, cf_.cy);
	a_ = a_ - r - carry;
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
	sum_flags(r, 1);
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
	dif_flags(r, 1);
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
	if (r2 == 0)
	{
		--r2;
		--r1;
	}
	else
		--r2;
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
	cycles_ += 10;
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
	uint16_t old_pc {pc_};
	uint8_t adjust {0x0};
	// if the value of the least significant 4 bits of the accumulator is
	// greater than 9 or if the AC flag is set, 6 is added to the accumulator
	if (((a_ & 0x0F) > 9) || cf_.ac)
		adjust += 0x06;
	// if the value of the most significant 4 bits of the accumulator is now
	// greater than 9 or if the AC flag is set, 6 is added to the most
	// significant 4 bits of the accumulator
	if ((((a_ & 0xF0) >> 4) > 9) || cf_.cy)
		adjust += 0x60;
	adi(adjust);
	cycles_ = old_cycles + 4;
	pc_ = old_pc;
}