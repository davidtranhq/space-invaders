#include "cpu.hpp"
	
namespace i8080
{

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
	uint16_t carry = sum ^ a ^ b;
	cf_.cy = (carry & (1 << 8));
    cf_.ac = (carry & (1 << 4));
}

void Cpu::dif_flags(uint8_t a, uint8_t b, uint8_t cy)
{
	uint16_t dif = static_cast<uint16_t>(a) - static_cast<uint16_t>(b) - cy;
	cf_.z = (dif == 0);
	cf_.s = ((dif & 0x80) == 0x80);
	cf_.p = (parity[static_cast<uint8_t>(a - b - cy)]);
	cf_.cy = (a < (b + cy)) ? 1 : 0;
	cf_.ac = ~(a ^ b ^ dif) & 0x10;
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
	cycles_ += 5;
}

void Cpu::dad(uint8_t r1, uint8_t r2)
{
	uint32_t sum = static_cast<uint32_t>(pair(h_, l_))
		+ static_cast<uint32_t>(pair(r1, r2));
	cf_.cy = (sum > 0xFFFF);
	uint16_t res = pair(h_, l_) + pair(r1, r2);
	l_ = static_cast<uint8_t>(res & 0xFF);
	h_ = static_cast<uint8_t>((res & 0xFF00) >> 8);
	cycles_ += 10;
}

void Cpu::dad(uint16_t r)
{
	uint32_t sum = static_cast<uint32_t>(r) + static_cast<uint32_t>(pair(h_, l_));
	cf_.cy = (sum > 0xFFFF);
	uint16_t res = pair(h_, l_) + r;
	l_ = static_cast<uint8_t>(res & 0xFF);
	h_ = static_cast<uint8_t>((res & 0xFF00) >> 8);
	cycles_ += 10;
}

void Cpu::daa()
{
	int old_cycles {cycles_};
	uint8_t old_carry {cf_.cy};
	uint16_t old_pc {pc_};
	uint8_t adjust {0x0};
	const uint8_t lsb = a_ & 0x0F;
	const uint8_t msb = a_ >> 4;
	// if the value of the least significant 4 bits of the accumulator is
	// greater than 9 or if the AC flag is set, 6 is added to the accumulator
	if ((lsb > 9) || cf_.ac)
		adjust += 0x06;
	// if the value of the most significant 4 bits of the accumulator is now
	// greater than 9 or if the AC flag is set, 6 is added to the most
	// significant 4 bits of the accumulator
	if ((msb > 9) || ((msb >= 9) && (lsb > 9)) || cf_.cy)
	{
		adjust += 0x60;
		old_carry = 1;
	}
	adi(adjust);
	cycles_ = old_cycles + 4;
	pc_ = old_pc;
	cf_.cy = old_carry;
}

// logical group

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

// branch group

void Cpu::jmp(uint8_t l, uint8_t h)
{
	uint16_t adr {pair(h, l)};
	pc_ = adr-1; // emulate_op increments the pc by one for each instruction
	cycles_ += 10;
}

void Cpu::j_condition(uint8_t cf, uint8_t l, uint8_t h)
{
	int old_cycles {cycles_};
	if (cf)
		jmp(l, h);
	else
		pc_ += 2;
	cycles_ = old_cycles + 10;
}

void Cpu::call(uint8_t l, uint8_t h)
{
	sp_ -= 2;
	mem_[sp_ + 1] = static_cast<uint8_t>((pc_+3) >> 8);
	mem_[sp_] = static_cast<uint8_t>((pc_+3) & 0xff);
	uint16_t adr {pair(h, l)};
	pc_ = adr-1;
	cycles_ += 17;
}

void Cpu::c_condition(uint8_t cf, uint8_t l, uint8_t h)
{
	if (cf)
	{
		call(l, h);
	}
	else
	{
		pc_ += 2;
		cycles_ += 11;
	}
}

void Cpu::ret()
{
	uint16_t pcl {static_cast<uint16_t>(mem_[sp_])};
	uint16_t pch = static_cast<uint16_t>(mem_[sp_ + 1]) << 8;
	pc_ = (pch | pcl) - 1;
	sp_ += 2;
	cycles_ += 10;
}

void Cpu::r_condition(uint8_t cf)
{
	if (cf)
	{
		ret();
		cycles_ += 1;
	}
	else
		cycles_ += 5 ;
}

void Cpu::rst(int n)
{
	mem_[sp_ - 1] = static_cast<uint8_t>((pc_) >> 8);
	mem_[sp_ - 2] = static_cast<uint8_t>((pc_) & 0xff);
	sp_ -= 2;
	pc_ = 8*n - 1;
	cycles_ += 11;
}

void Cpu::pchl()
{
	uint16_t adr {pair(h_, l_)};
	pc_ = adr-1;
	cycles_ += 5;
}

// stack, special, machine io

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
	cf_.cy = word & 1;
	cf_.p = (word >> 2) & 1;
	cf_.ac = (word >> 4) & 1;
	cf_.z = (word >> 6) & 1;
	cf_.s = (word >> 7) & 1;
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
	mem_[sp_ + 1] = tmp;
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

}
