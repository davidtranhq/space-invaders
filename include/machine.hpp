#pragma once

#include <chrono>
#include <SDL.h>

#include "cpu.hpp"
#include "audio.hpp"

#define SCREEN_HEIGHT 256
#define SCREEN_WIDTH 224
	
namespace space_invaders
{


class Machine
{
	public:
	Machine();
	
	void run();
	bool load_program(const std::string &in, uint16_t off = 0);
	
	uint8_t in(uint8_t port);
	void out(uint8_t port, uint8_t val);
	void key_down(SDL_Keycode k);
	void key_up(SDL_Keycode k);
	void update_buffer();
	void update_screen();
	
	private:
	i8080::Cpu cpu_;
	std::array<uint8_t, 0x10000> memory_;
	
	uint8_t shift0 {0};
	uint8_t shift1 {0};
	uint8_t shift_offset {0};
	uint8_t inp1_ {0};
	uint8_t inp2_ {0};
	uint8_t sound1_ {0}, last_sound1_ {0};
	uint8_t sound2_ {0}, last_sound2_ {0};
	
	bool done_ {false};
	std::array<std::array<std::array<uint8_t, 3>, SCREEN_WIDTH>, SCREEN_HEIGHT> screen_buf_ {};
	SDL_Window *window_;
	SDL_Surface *disp_;
	std::array<Wav, 9> sounds_;
	
	void execute_cpu(long cyc);
	void play_sound();
	
};

}