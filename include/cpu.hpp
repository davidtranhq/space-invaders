#pragma once

#include <cstdint>
#include <array>
#include <iostream>
#include <functional>

namespace dav
{

namespace i8080
{
	
class Test;

struct Condition_flags
{
	bool z, s, p, cy, ac;
};

class Cpu
{
	public:
	
	explicit Cpu(std::array<uint8_t, 0x10000> &a);
	explicit Cpu(std::array<uint8_t, 0x10000> &a,
				 std::function<uint8_t(uint8_t)> in,
				 std::function<void(uint8_t, uint8_t)> out);

	void interrupt(uint8_t op);
	int emulate_op();
	
	void set_pc(uint16_t x);
	uint16_t pc() const;
	int cycles() const;
	uint8_t b() const;
	uint8_t c() const;
	uint8_t d() const;
	uint8_t e() const;
	uint8_t h() const;
	uint8_t l() const;
	uint16_t pair(uint8_t r1, uint8_t r2);
	
	// debug functions
	#ifdef DEBUG
		void debug_step(int x);
		void debug_info(std::ostream &os = std::cout);
		size_t debug_instructions {0};
		friend class i8080::Test;
	#endif
	
	private:
	
	std::function<uint8_t(uint8_t)> in_handle_ {};
	std::function<void(uint8_t, uint8_t)> out_handle_ {};
	
	
	uint8_t b_ {0}, c_ {0}, d_ {0}, e_ {0}, h_ {0}, l_ {0}, a_ {0}; // registers
	uint16_t sp_ {0}, pc_ {0}; // stack pointer, program counter
	Condition_flags cf_ {}; // condition flags
	std::array<uint8_t, 0x10000> &mem_; // 64k ram addressing
	int cycles_ {0};
	
	bool int_enabled_ {false};
	bool int_pending_ {false};
	uint8_t int_op_ {0};
	bool halted_ {false};
	
	// instructions
	void mov(uint8_t &r1, uint8_t r2);
	void mov_r(uint8_t &r);
	void mov_m(uint8_t r);
	void mvi(uint8_t &r, uint8_t d);
	void mvi_m(uint8_t d);
	void lxi(uint8_t &r1, uint8_t &r2, uint8_t l, uint8_t h);
	void lxi(uint16_t &r, uint8_t l, uint8_t h);
	void lda(uint8_t l, uint8_t h);
	void sta(uint8_t l, uint8_t h);
	void lhld(uint8_t l, uint8_t h);
	void shld(uint8_t l, uint8_t h);
	void ldax(uint8_t r1, uint8_t r2);
	void stax(uint8_t r1, uint8_t r2);
	void xchg();
	// arithmetic group
	void add(uint8_t r);
	void add_m();
	void adi(uint8_t d);
	void adc(uint8_t r);
	void adc_m();
	void aci(uint8_t d);
	void sub(uint8_t r);
	void sub_m();
	void sui(uint8_t d);
	void sbb(uint8_t r);
	void sbb_m();
	void sbi(uint8_t d);
	void inr(uint8_t &r);
	void inr_m();
	void dcr(uint8_t &r);
	void dcr_m();
	void inx(uint8_t &r1, uint8_t &r2);
	void inx(uint16_t &r);
	void dcx(uint8_t &r1, uint8_t &r2);
	void dcx(uint16_t &r);
	void dad(uint8_t r1, uint8_t r2);
	void dad(uint16_t r);
	void daa();
	// logical group
	void ana(uint8_t r);
	void ana_m();
	void ani(uint8_t r);
	void xra(uint8_t r);
	void xra_m();
	void xri(uint8_t d);
	void ora(uint8_t r);
	void ora_m();
	void ori(uint8_t d);
	void cmp(uint8_t r);
	void cmp_m();
	void cpi(uint8_t d);
	void rlc();
	void rrc();
	void ral();
	void rar();
	void cma();
	void cmc();
	void stc();
	// branch group
	void jmp(uint8_t l, uint8_t h);
	void j_condition(uint8_t cf, uint8_t l, uint8_t h);
	void call(uint8_t l, uint8_t h);
	void c_condition(uint8_t cf, uint8_t l, uint8_t h);
	void ret();
	void r_condition(uint8_t cf);
	void rst(int n);
	void pchl();
	// stack group
	void push(uint8_t r1, uint8_t r2);
	void push_psw();
	void pop(uint8_t &r1, uint8_t &r2);
	void pop_psw();
	void xthl();
	void sphl();
	void in(uint8_t port);
	void out(uint8_t port);
	void ei();
	void di();
	void hlt();
	void nop();
	
	// helper functions
	void set_flags(uint8_t res);
	void sum_flags(uint8_t a, uint8_t b, uint8_t cy = 0);
	void dif_flags(uint8_t a, uint8_t b, uint8_t cy = 0);
};

}

}