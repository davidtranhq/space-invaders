#pragma once

#include <chrono>
#include <SDL.h>

#include "cpu.hpp"

#define SCREEN_HEIGHT 256
#define SCREEN_WIDTH 224

namespace dav
{
	
namespace i8080
{


class Machine
{
	public:
	Machine();
	
	void run();
	void load_program(const std::string &in, uint16_t off = 0);
	
	uint8_t in(uint8_t port);
	void out(uint8_t port, uint8_t val);
	void key_down(SDL_Keycode k);
	void key_up(SDL_Keycode k);
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
	std::array<std::array<std::array<uint8_t, 3>, SCREEN_WIDTH>, SCREEN_HEIGHT> screen_buf_ {};
	SDL_Window *window_;
	SDL_Surface *disp_;
	
	void execute_cpu(long cyc);
	
};

}

}