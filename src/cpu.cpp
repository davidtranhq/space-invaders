#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>

#include "cpu.hpp"

namespace dav
{
	
namespace i8080
{

Cpu::Cpu() {}

Cpu::Cpu(std::function<uint8_t(uint8_t)> in, std::function<void(uint8_t, uint8_t)> out) 
	: in_handle_(in), out_handle_(out)
{}

void Cpu::load_program(const std::string &path, uint16_t offset)
{
	std::ifstream f {path, std::ios::binary | std::ios::in};
	f >> std::noskipws;
	std::copy
	(
		std::istreambuf_iterator<char>(f),
		std::istreambuf_iterator<char>(),
		mem_.begin() + offset
	);
	pc_ = offset;
	#ifdef DEBUG
		std::cout << "File loaded, size " << std::dec << f.tellg() << '\n';
	#endif
	mem_[368] = 0x7;
}

uint16_t Cpu::pair(uint8_t r1, uint8_t r2)
{
	return static_cast<uint16_t>(r1) << 8 | static_cast<uint16_t>(r2);
}

const std::array<uint8_t, 0x10000> &Cpu::memory() const
{
	return mem_;
}

const int Cpu::cycles() const
{
	return cycles_;
}

void Cpu::interrupt(uint8_t op)
{
	halted_ = false;
	if (int_enabled_)
	{
		int_enabled_ = false;
		int_pending_ = true;
		int_op_ = op;
	}
}

int Cpu::emulate_op()
{
	if (halted_)
		return -1;
	uint8_t *opcode {&mem_[pc_]};
	if (int_pending_)
	{
		opcode = &int_op_;
		int_pending_ = false;
		int_enabled_ = true;
	}
	switch (*opcode)
	{
		case 0x00: nop();
			break; 
		case 0x01: lxi(b_, c_, opcode[1], opcode[2]);
			break; 
		case 0x02: stax(b_, c_);
			break; 
		case 0x03: inx(b_, c_);
			break; 
		case 0x04: inr(b_);
			break; 
		case 0x05: dcr(b_);
			break;
		case 0x06: mvi(b_, opcode[1]);
			break;
		case 0x07: rlc();
			break;
		case 0x08: nop();
			break;
		case 0x09: dad(b_, c_);
			break;
		case 0x0A: ldax(b_, c_);
			break;
		case 0x0B: dcx(b_, c_);
			break;
		case 0x0C: inr(c_);
			break;
		case 0x0D: dcr(c_);
			break;
		case 0x0E: mvi(c_, opcode[1]);
			break;
		case 0x0F: rrc();
			break;
		case 0x10: nop();
			break;
		case 0x11: lxi(d_, e_, opcode[1], opcode[2]);
			break;
		case 0x12: stax(d_, e_);
			break;
		case 0x13: inx(d_, e_);
			break; 
		case 0x14: inr(d_);
			break; 
		case 0x15: dcr(d_);
			break; 
		case 0x16: mvi(d_, opcode[1]);
			break; 
		case 0x17: ral();
			break; 
		case 0x18: nop();
			break;
		case 0x19: dad(d_, e_);
			break; 
		case 0x1A: ldax(d_, e_);
			break; 
		case 0x1B: dcx(d_, e_);
			break; 
		case 0x1C: inr(e_);
			break; 
		case 0x1D: dcr(e_);
			break; 
		case 0x1E: mvi(e_, opcode[1]);
			break; 
		case 0x1F: rar();
			break; 
		case 0x20: nop();
			break;
		case 0x21: lxi(h_, l_, opcode[1], opcode[2]);
			break; 
		case 0x22: shld(opcode[1], opcode[2]);
			break; 
		case 0x23: inx(h_, l_);
			break; 
		case 0x24: inr(h_);
			break; 
		case 0x25: dcr(h_);
			break; 
		case 0x26: mvi(h_, opcode[1]);
			break; 
		case 0x27: daa();
			break; 
		case 0x28: nop();
			break; 
		case 0x29: dad(h_, l_);
			break; 
		case 0x2A: lhld(opcode[1], opcode[2]);
			break; 
		case 0x2B: dcx(h_, l_);
			break; 
		case 0x2C: inr(l_);
			break; 
		case 0x2D: dcr(l_);
			break; 
		case 0x2E: mvi(l_, opcode[1]);
			break; 
		case 0x2F: cma();
			break;
		case 0x30: nop();
			break;
		case 0x31: lxi(sp_, opcode[1], opcode[2]);
			break;
		case 0x32: sta(opcode[1], opcode[2]);
			break;
		case 0x33: inx(sp_);
			break;
		case 0x34: inr_m();
			break;
		case 0x35: dcr_m();
			break;
		case 0x36: mvi_m(opcode[1]);
			break;
		case 0x37: stc();
			break;
		case 0x38: nop();
			break;
		case 0x39: dad(sp_);
			break;
		case 0x3A: lda(opcode[1], opcode[2]);
			break;
		case 0x3B: dcx(sp_);
			break;
		case 0x3C: inr(a_);
			break;
		case 0x3D: dcr(a_);
			break;
		case 0x3E: mvi(a_, opcode[1]);
			break;
		case 0x3F: cmc();
			break;
		case 0x40: mov(b_, b_);
			break;
		case 0x41: mov(b_, c_);
			break;
		case 0x42: mov(b_, d_);
			break; 
		case 0x43: mov(b_, e_);
			break;
		case 0x44: mov(b_, h_);
			break;
		case 0x45: mov(b_, l_);
			break;
		case 0x46: mov_r(b_);
			break;
		case 0x47: mov(b_, a_);
			break;
		case 0x48: mov(c_, b_);
			break; 
		case 0x49: mov(c_, c_);
			break; 
		case 0x4A: mov(c_, d_);
			break; 
		case 0x4B: mov(c_, e_);
			break; 
		case 0x4C: mov(c_, h_);
			break; 
		case 0x4D: mov(c_, l_);
			break; 
		case 0x4E: mov_r(c_);
			break; 
		case 0x4F: mov(c_, a_);
			break;
		case 0x50: mov(d_, b_);
			break; 
		case 0x51: mov(d_, c_);
			break; 
		case 0x52: mov(d_, d_);
			break; 
		case 0x53: mov(d_, e_);
			break; 
		case 0x54: mov(d_, h_);
			break; 
		case 0x55: mov(d_, l_);
			break; 
		case 0x56: mov_r(d_);
			break; 
		case 0x57: mov(d_, a_);
			break;
		case 0x58: mov(e_, b_);
			break; 
		case 0x59: mov(e_, c_);
			break; 
		case 0x5A: mov(e_, d_);
			break; 
		case 0x5B: mov(e_, e_);
			break; 
		case 0x5C: mov(e_, h_);
			break; 
		case 0x5D: mov(e_, l_);
			break; 
		case 0x5E: mov_r(e_);
			break; 
		case 0x5F: mov(e_, a_);
			break;
		case 0x60: mov(h_, h_);
			break; 
		case 0x61: mov(h_, c_);
			break; 
		case 0x62: mov(h_, d_);
			break; 
		case 0x63: mov(h_, e_);
			break; 
		case 0x64: mov(h_, h_);
			break; 
		case 0x65: mov(h_, l_);
			break; 
		case 0x66: mov_r(h_);
			break; 
		case 0x67: mov(h_, a_);
			break;
		case 0x68: mov(l_, b_);
			break; 
		case 0x69: mov(l_, c_);
			break; 
		case 0x6A: mov(l_, d_);
			break; 
		case 0x6B: mov(l_, e_);
			break; 
		case 0x6C: mov(l_, h_);
			break; 
		case 0x6D: mov(l_, l_);
			break; 
		case 0x6E: mov_r(l_);
			break; 
		case 0x6F: mov(l_, a_);
			break;
		case 0x70: mov_m(b_);
			break;
		case 0x71: mov_m(c_);
			break;
		case 0x72: mov_m(d_);
			break;
		case 0x73: mov_m(e_);
			break;
		case 0x74: mov_m(h_);
			break;
		case 0x75: mov_m(l_);
			break;
		case 0x76: hlt();
			break;
		case 0x77: mov_m(a_);
			break;
		case 0x78: mov(a_, b_);
			break; 
		case 0x79: mov(a_, c_);
			break; 
		case 0x7A: mov(a_, d_);
			break; 
		case 0x7B: mov(a_, e_);
			break; 
		case 0x7C: mov(a_, h_);
			break; 
		case 0x7D: mov(a_, l_);
			break; 
		case 0x7E: mov_r(a_);
			break; 
		case 0x7F: mov(a_, a_);
			break;
		case 0x80: add(b_);
			break;
		case 0x81: add(c_);
			break;
		case 0x82: add(d_);
			break;
		case 0x83: add(e_);
			break;
		case 0x84: add(h_);
			break;
		case 0x85: add(l_);
			break;
		case 0x86: add_m();
			break;
		case 0x87: adc(a_);
			break;
		case 0x88: adc(b_);
			break; 
		case 0x89: adc(c_);
			break; 
		case 0x8A: adc(d_);
			break; 
		case 0x8B: adc(e_);
			break; 
		case 0x8C: adc(h_);
			break; 
		case 0x8D: adc(l_);
			break; 
		case 0x8E: adc_m();
			break; 
		case 0x8F: adc(a_);
			break;
		case 0x90: sub(b_);
			break; 
		case 0x91: sub(c_);
			break; 
		case 0x92: sub(d_);
			break; 
		case 0x93: sub(e_);
			break; 
		case 0x94: sub(h_);
			break; 
		case 0x95: sub(l_);
			break; 
		case 0x96: sub_m();
			break; 
		case 0x97: sub(a_);
			break;
		case 0x98: sbb(b_);
			break; 
		case 0x99: sbb(c_);
			break; 
		case 0x9A: sbb(d_);
			break; 
		case 0x9B: sbb(e_);
			break; 
		case 0x9C: sbb(h_);
			break; 
		case 0x9D: sbb(l_);
			break; 
		case 0x9E: sbb_m();
			break; 
		case 0x9F: sbb(a_);
			break;
		case 0xA0: ana(b_);
			break; 
		case 0xA1: ana(c_);
			break; 
		case 0xA2: ana(d_);
			break; 
		case 0xA3: ana(e_);
			break; 
		case 0xA4: ana(h_);
			break; 
		case 0xA5: ana(l_);
			break; 
		case 0xA6: ana_m();
			break; 
		case 0xA7: ana(a_);
			break;
		case 0xA8: xra(b_);
			break; 
		case 0xA9: xra(c_);
			break; 
		case 0xAA: xra(d_);
			break; 
		case 0xAB: xra(e_);
			break; 
		case 0xAC: xra(h_);
			break; 
		case 0xAD: xra(l_);
			break; 
		case 0xAE: xra_m();
			break; 
		case 0xAF: xra(a_);
			break;
		case 0xB0: ora(b_);
			break; 
		case 0xB1: ora(c_);
			break; 
		case 0xB2: ora(d_);
			break; 
		case 0xB3: ora(e_);
			break; 
		case 0xB4: ora(h_);
			break; 
		case 0xB5: ora(l_);
			break; 
		case 0xB6: ora_m();
			break; 
		case 0xB7: ora(a_);
			break;
		case 0xB8: cmp(b_);
			break; 
		case 0xB9: cmp(c_);
			break; 
		case 0xBA: cmp(d_);
			break; 
		case 0xBB: cmp(e_);
			break; 
		case 0xBC: cmp(h_);
			break; 
		case 0xBD: cmp(l_);
			break; 
		case 0xBE: cmp_m();
			break; 
		case 0xBF: cmp(a_);
			break;
		case 0xC0: r_condition(!cf_.z);
			break;
		case 0xC1: pop(b_, c_);
			break;
		case 0xC2: j_condition(!cf_.z, opcode[1], opcode[2]);
			break;
		case 0xC3: jmp(opcode[1], opcode[2]);
			break;
		case 0xC4: c_condition(!cf_.z, opcode[1], opcode[2]);
			break;
		case 0xC5: push(b_, c_);
			break;
		case 0xC6: adi(opcode[1]);
			break;
		case 0xC7: rst(0);
			break;
		case 0xC8: r_condition(cf_.z);
			break;
		case 0xC9: ret();
			break;
		case 0xCA: j_condition(cf_.z, opcode[1], opcode[2]);
			break;
		case 0xCB: nop();
			break;
		case 0xCC: c_condition(cf_.z, opcode[1], opcode[2]);
			break;
		case 0xCD: call(opcode[1], opcode[2]);
			break;
		case 0xCE: aci(opcode[1]);
			break;
		case 0xCF: rst(1);
			break;
		case 0xD0: r_condition(!cf_.cy);
			break;
		case 0xD1: pop(d_, e_);
			break;
		case 0xD2: j_condition(!cf_.cy, opcode[1], opcode[2]);
			break;
		case 0xD3: out(opcode[1]);
			break;
		case 0xD4: c_condition(!cf_.cy, opcode[1], opcode[2]);
			break;
		case 0xD5: push(d_, e_);
			break;
		case 0xD6: sui(opcode[1]);
			break;
		case 0xD7: rst(2);
			break;
		case 0xD8: r_condition(cf_.cy);
			break;
		case 0xD9: nop();
			break;
		case 0xDA: j_condition(cf_.cy, opcode[1], opcode[2]);
			break;
		case 0xDB: in(opcode[1]);
			break;
		case 0xDC: c_condition(cf_.cy, opcode[1], opcode[2]);
			break;
		case 0xDD: nop();
			break;
		case 0xDE: sbi(opcode[1]);
			break;
		case 0xDF: rst(3);
			break;
		case 0xE0: r_condition(!cf_.p);
			break;
		case 0xE1: pop(h_, l_);
			break;
		case 0xE2: j_condition(!cf_.p, opcode[1], opcode[2]);
			break;
		case 0xE3: xthl();
			break;
		case 0xE4: c_condition(!cf_.p, opcode[1], opcode[2]);
			break;
		case 0xE5: push(h_, l_);
			break;
		case 0xE6: ani(opcode[1]);
			break;
		case 0xE7: rst(4);
			break;
		case 0xE8: r_condition(cf_.p);
			break;
		case 0xE9: pchl();
			break;
		case 0xEA: j_condition(cf_.p, opcode[1], opcode[2]);
			break;
		case 0xEB: xchg();
			break;
		case 0xEC: c_condition(cf_.p, opcode[1], opcode[2]);
			break;
		case 0xED: nop();
			break;
		case 0xEE: xri(opcode[1]);
			break;
		case 0xEF: rst(5);
			break;
		case 0xF0: r_condition(cf_.p);
			break;
		case 0xF1: pop_psw();
			break;
		case 0xF2: j_condition(cf_.p, opcode[1], opcode[2]);
			break;
		case 0xF3: di();
			break;
		case 0xF4: c_condition(cf_.p, opcode[1], opcode[2]);
			break;
		case 0xF5: push_psw();
			break;
		case 0xF6: ori(opcode[1]);
			break;
		case 0xF7: rst(6);
			break;
		case 0xF8: r_condition(cf_.s);
			break;
		case 0xF9: sphl();
			break;
		case 0xFA: j_condition(cf_.s, opcode[1], opcode[2]);
			break;
		case 0xFB: ei();
			break;
		case 0xFC: c_condition(cf_.s, opcode[1], opcode[2]);
			break;
		case 0xFD: nop();
			break;
		case 0xFE: cpi(opcode[1]);
			break;
		case 0xFF: rst(7);
			break;
	}
	#ifdef DEBUG
		++debug_instructions;
	#endif
	++pc_;
	return *opcode;
}

const std::array<std::pair<std::string, int>, 256> op_codes
{{
	{"NOP", 1},
	{"LXI B, D16", 3},
	{"STAX B", 1},
	{"INX B", 1},
	{"INR B", 1},
	{"DCR B", 1},
	{"MVI B, D8", 2},
	{"RLC", 1},
	{"NOP", 1},
	{"DAD B", 1},
	{"LDAX B", 1},
	{"DCX B", 1},
	{"INR C", 1},
	{"DCR C", 1},
	{"MVI C, D8", 2},
	{"RRC", 1},
	{"NOP", 1},
	{"LXI D, D16", 3},
	{"STAX D", 1},
	{"INX D", 1},
	{"INR D", 1},
	{"DCR D", 1},
	{"MVI D, D8", 2},
	{"RAL", 1},
	{"NOP", 1},
	{"DAD D", 1},
	{"LDAX D", 1},
	{"DCX D", 1},
	{"INR E", 1},
	{"DCR E", 1},
	{"MVI E, D8", 2},
	{"RAR", 1},
	{"NOP", 1},
	{"LXI H, D16", 3},
	{"SHLD adr", 3},
	{"INX H", 1},
	{"INR H", 1},
	{"DCR H", 1},
	{"MVI H, D8", 2},
	{"DAA", 1},
	{"NOP", 1},
	{"DAD H", 1},
	{"LHLD adr", 3},
	{"DCX H", 1},
	{"INR L", 1},
	{"DCR L", 1},
	{"MVI L, D8", 2},
	{"CMA", 1},
	{"NOP", 1},
	{"LXI SP, D16", 3},
	{"STA adr", 3},
	{"INX SP", 1},
	{"INR M", 1},
	{"DCR M", 1},
	{"MVI M, D8", 2},
	{"STC", 1},
	{"NOP", 1},
	{"DAD SP", 1},
	{"LDA adr", 3},
	{"DCX SP", 1},
	{"INR A", 1},
	{"DCR A", 1},
	{"MVI A, D8", 2},
	{"CMC", 1},
	{"MOV B, B", 1},
	{"MOV B, C", 1},
	{"MOV B, D", 1},
	{"MOV B, E", 1},
	{"MOV B, H", 1},
	{"MOV B, L", 1},
	{"MOV B, M", 1},
	{"MOV B, A", 1},
	{"MOV C, B", 1},
	{"MOV C, C", 1},
	{"MOV C, D", 1},
	{"MOV C, E", 1},
	{"MOV C, H", 1},
	{"MOV C, L", 1},
	{"MOV C, M", 1},
	{"MOV C, A", 1},
	{"MOV D, B", 1},
	{"MOV D, C", 1},
	{"MOV D, D", 1},
	{"MOV D, E", 1},
	{"MOV D, H", 1},
	{"MOV D, L", 1},
	{"MOV D, M", 1},
	{"MOV D, A", 1},
	{"MOV E, B", 1},
	{"MOV E, C", 1},
	{"MOV E, D", 1},
	{"MOV E, E", 1},
	{"MOV E, H", 1},
	{"MOV E, M", 1},
	{"MOV E, L", 1},
	{"MOV E, A", 1},
	{"MOV H, B", 1},
	{"MOV H, C", 1},
	{"MOV H, D", 1},
	{"MOV H, E", 1},
	{"MOV H, H", 1},
	{"MOV H, L", 1},
	{"MOV H, M", 1},
	{"MOV H, A", 1},
	{"MOV L, B", 1},
	{"MOV L, C", 1},
	{"MOV L, D", 1},
	{"MOV L, E", 1},
	{"MOV L, H", 1},
	{"MOV L, L", 1},
	{"MOV L, M", 1},
	{"MOV L, A", 1},
	{"MOV M, B", 1},
	{"MOV M, C", 1},
	{"MOV M, D", 1},
	{"MOV M, E", 1},
	{"MOV M, H", 1},
	{"MOV M, L", 1},
	{"HLT", 1},
	{"MOV M, A", 1},
	{"MOV A, B", 1},
	{"MOV A, C", 1},
	{"MOV A, D", 1},
	{"MOV A, E", 1},
	{"MOV A, H", 1},
	{"MOV A, L", 1},
	{"MOV A, M", 1},
	{"MOV A, A", 1},
	{"ADD B", 1},
	{"ADD C", 1},
	{"ADD D", 1},
	{"ADD E", 1},
	{"ADD H", 1},
	{"ADD L", 1},
	{"ADD M", 1},
	{"ADD A", 1},
	{"ADC B", 1},
	{"ADC C", 1},
	{"ADC D", 1},
	{"ADC E", 1},
	{"ADC H", 1},
	{"ADC L", 1},
	{"ADC M", 1},
	{"ADC A", 1},
	{"SUB B", 1},
	{"SUB C", 1},
	{"SUB D", 1},
	{"SUB E", 1},
	{"SUB H", 1},
	{"SUB L", 1},
	{"SUB M", 1},
	{"SUB A", 1},
	{"SBB B", 1},
	{"SBB C", 1},
	{"SBB D", 1},
	{"SBB E", 1},
	{"SBB H", 1},
	{"SBB L", 1},
	{"SBB M", 1},
	{"SBB A", 1},
	{"ANA B", 1},
	{"ANA C", 1},
	{"ANA D", 1},
	{"ANA E", 1},
	{"ANA H", 1},
	{"ANA L", 1},
	{"ANA M", 1},
	{"ANA A", 1},
	{"XRA B", 1},
	{"XRA C", 1},
	{"XRA D", 1},
	{"XRA E", 1},
	{"XRA H", 1},
	{"XRA L", 1},
	{"XRA M", 1},
	{"XRA A", 1},
	{"ORA B", 1},
	{"ORA C", 1},
	{"ORA D", 1},
	{"ORA E", 1},
	{"ORA H", 1},
	{"ORA L", 1},
	{"ORA M", 1},
	{"ORA A", 1},
	{"CMP B", 1},
	{"CMP C", 1},
	{"CMP D", 1},
	{"CMP E", 1},
	{"CMP H", 1},
	{"CMP L", 1},
	{"CMP M", 1},
	{"CMP A", 1},
	{"RNZ", 1},
	{"POP B", 1},
	{"JNZ adr", 3},
	{"JMP adr", 3},
	{"CNZ adr", 3},
	{"PUSH B", 1},
	{"ADI D8", 2},
	{"RST 0", 1},
	{"RZ", 1},
	{"RET", 1},
	{"JZ adr", 3},
	{"NOP", 1},
	{"CZ adr", 3},
	{"CALL adr", 3},
	{"ACI D8", 2},
	{"RST 1", 1},
	{"RNC 1", 1},
	{"POP D", 1},
	{"JNC adr", 3},
	{"OUT D8", 2},
	{"CNC adr", 3},
	{"PUSH D", 1},
	{"SUI D8", 2},
	{"RST 2", 1},
	{"RC", 1},
	{"NOP", 1},
	{"JC adr", 3},
	{"IN D8", 2},
	{"CC adr", 3},
	{"NOP", 1},
	{"SBI D8", 2},
	{"RST 3", 1},
	{"RPO", 1},
	{"POP H", 1},
	{"JPO adr", 3},
	{"XTHL", 1},
	{"CPO adr", 3},
	{"PUSH H", 1},
	{"ANI D8", 2},
	{"RST 4", 1},
	{"RPE", 1},
	{"PHCL", 1},
	{"JPE adr", 3},
	{"XCHG", 1},
	{"CPE adr", 3},
	{"NOP", 1},
	{"XRI D8", 2},
	{"RST 5", 1},
	{"RP", 1},
	{"POP PSW", 1},
	{"JP adr", 3},
	{"DI", 1},
	{"CP adr", 3},
	{"PUSH PSW", 1},
	{"ORI D8", 2},
	{"RST 6", 1},
	{"RM", 1},
	{"SPHL", 1},
	{"JM adr", 3},
	{"El", 1},
	{"CM adr", 3},
	{"NOP", 1},
	{"CPI D8", 2},
	{"RST 7", 1},
}};

#ifdef DEBUG

void Cpu::debug_step(int x)
{
	for (int i = 0; i < x; ++i)
	{
		debug_info(std::cerr);
		emulate_op();
	}
}

void Cpu::debug_info(std::ostream &os)
{
	os << "Instructions Ran: " << std::dec << debug_instructions << '\n';
	os << "Program Counter: " << std::hex << std::uppercase
		<< std::setfill('0') << std::setw(4)<< pc_ << '\n';
	os << "Memory Immediate: 0x" 
		<< std::setw(2) << static_cast<int>(mem_[pc_]) << '\n';
	os << "Instruction: " << op_codes[mem_[pc_]].first;
	for (int i = 1; i < op_codes[mem_[pc_]].second; ++i)
		os << ' ' << static_cast<int>(mem_[pc_+i]);
	os << '\n' << std::setw(4);
	os << "Registers (B/C/D/E/H/L/A): "  
		<< std::setw(2) << static_cast<int>(b_) << ' ' 
		<< std::setw(2) << static_cast<int>(c_) << ' '
		<< std::setw(2) << static_cast<int>(d_) << ' ' 
		<< std::setw(2) << static_cast<int>(e_) << ' '
		<< std::setw(2) << static_cast<int>(h_) << ' ' 
		<< std::setw(2) << static_cast<int>(l_) << ' '
		<< std::setw(2) << static_cast<int>(a_) << '\n';
	os << "Memory at HL (" << std::setw(4) << static_cast<int>(pair(h_, l_)) << "): "
		<< std::setw(2) << static_cast<int>(mem_[pair(h_, l_)]) << '\n';
	os << "Flags (Z/S/P/C/AC): "
		<< static_cast<int>(cf_.z) << ' ' << static_cast<int>(cf_.s) << ' '
		<< static_cast<int>(cf_.p) << ' ' << static_cast<int>(cf_.cy) << ' '
		<< static_cast<int>(cf_.ac) << ' ' << '\n';
	os << "Stack Pointer: " << static_cast<int>(sp_) << '\n';
	os << "Cycles: " << std::dec << cycles_ << '\n';
	os << "Interrupt enabled: " << std::boolalpha << int_enabled_ << '\n';
	os << "Interrupt op: " << std::hex << static_cast<uint8_t>(int_op_) << '\n';
	os << std::setfill('-') << std::setw(20) << ' ' << '\n';
}

#endif

}

}