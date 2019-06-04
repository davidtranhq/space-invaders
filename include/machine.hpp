#pragma once

#include <chrono>
#include <SDL.h>

#include "cpu.hpp"

#define SCREEN_HEIGHT 256
#define SCREEN_WIDTH 224

namespace i8080
{
	class Machine;
	class Cpu;
	enum class KeyPress;
};

enum class i8080::KeyPress
{
	left, right, up, down
};

class i8080::Machine
{
	public:
	Machine();
	
	void run();
	void load_program(const std::string &in, uint16_t off = 0);
	
	uint8_t in(uint8_t port);
	void out(uint8_t port, uint8_t val);
	void key_down(KeyPress k);
	void key_up(KeyPress k);
	void update_buffer();
	void update_screen();
	
	private:
	Cpu cpu_;
	
	uint8_t shft_in_ {0};
	uint8_t shft_data_ {0};
	uint8_t shft_amnt_ {0};
	uint8_t shft_reg_ {0};
	uint8_t inp1_ {0};
	
	bool done_ {false};
	bool first_execution_ {true};
	bool vbl_ {false}; // interrupt at end of frame, if false, interrupt is middle of frame
	std::chrono::high_resolution_clock::time_point last_frame_;
	std::chrono::high_resolution_clock::time_point last_cpu_;
	std::chrono::high_resolution_clock::time_point last_int_;
	std::array<std::array<std::array<uint8_t, 3>, SCREEN_WIDTH>, SCREEN_HEIGHT> screen_buf_ {};
	SDL_Window *window_;
	SDL_Surface *disp_;
	
	void check_interrupt();
	void execute_cpu();
	
};