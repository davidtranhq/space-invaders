#include <chrono>

#include "machine.hpp"

using i8080::Machine;
using namespace std::chrono;
using Clock = high_resolution_clock;
	
Machine::Machine()
	: cpu_
	( 
		[this](uint8_t o) { return this->in(o); },
		[this](uint8_t p, uint8_t val) { this->out(p, val); }
	),
	last_cpu_ {std::chrono::high_resolution_clock::now()},
	last_int_ {std::chrono::high_resolution_clock::now()},
	window_ {SDL_CreateWindow("Space Invaders!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0)},
	disp_ {SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0)}
{
	if (!window_)
	{
		std::cerr << "Could not create SDL_Window!\n";
		throw;
	}
	if (!disp_)
	{
		std::cerr << "Could not create SDL_Surface!\n";
		throw;
	}
}

void Machine::check_interrupt()
{
	if (duration_cast<microseconds>(Clock::now() - last_int_)
		> 8333us) // interrupt every half a frame or 1666... us divided by 2
	{
		if (vbl_) // end of screen interrupt
			cpu_.interrupt(0xD7); // RST 2
		else // middle of screen interrupt
			cpu_.interrupt(0xCF); // RST 1
		vbl_ = !vbl_;
	}
}

void Machine::execute_cpu()
{
	auto time_since_last_cpu_ 
		= duration_cast<microseconds>(Clock::now() - last_cpu_).count();
	// cpu runs at 2Mhz
	uint64_t cycles_to_catch_up = time_since_last_cpu_ * 2;
	uint64_t cycles_executed {0};
	uint64_t start_cycles {0}, end_cycles {0};
	while (cycles_executed < cycles_to_catch_up)
	{
		start_cycles = cpu_.cycles();
		cpu_.emulate_op();
		end_cycles = cpu_.cycles();
		cycles_executed += (end_cycles - start_cycles);
	}
	last_cpu_ = Clock::now();
}

void Machine::run()
{
	SDL_Event e;
	while (!done_)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				done_ = true;
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_d:
						cpu_.debug_info();
						break;
				}
			}
		}
		if (first_execution_)
		{
			first_execution_ = false;
			last_cpu_ = Clock::now();
			last_frame_ = last_cpu_;
			last_int_ = last_cpu_;
		}
		execute_cpu();
		if (duration_cast<milliseconds>(Clock::now() - last_frame_).count() > 16.6667)
		{
			update_screen();
			last_frame_ = Clock::now();
		}
	}
}

void Machine::update_screen()
{
	uint32_t *pix {(uint32_t *) disp_->pixels};
	int i {0x2400}; // start of video ram
	for (int col {0}; col < SCREEN_WIDTH; ++col)
	{
		for (int row {SCREEN_HEIGHT}; row > 0; row -= 8)
		{
			for (int j {0}; j < 8; ++j)
			{	
				int idx = (row - j) * SCREEN_WIDTH + col;
				if (cpu_.memory()[i] & 1 >> j)
					pix[idx] = 0xFFFFFF;
				else
					pix[idx] = 0x000000;
			}
			++i;
		}
	}
	SDL_Surface *winsurf = SDL_GetWindowSurface(window_);
	SDL_BlitScaled(disp_, NULL, winsurf, NULL);
	if (SDL_UpdateWindowSurface(window_))
		std::cerr << SDL_GetError();
}

void Machine::load_program(const std::string &in, uint16_t off)
{
	cpu_.load_program(in, off);
}

uint8_t Machine::in(uint8_t port)
{
	uint8_t a;
	switch (port)
	{
		case 0:
			return 1;
		case 1:
			return 0;
		case 3:
		{
			a = ((shft_reg_ >> (8-shft_amnt_)) & 0xff);
		}
		break;
	}
	return a;
}

void Machine::out(uint8_t port, uint8_t val)
{
	switch (port)
	{
		case 2:
			shft_amnt_ = val & 0x07;
			break;
		case 4:
		{
			uint16_t v {static_cast<uint16_t>(val)};
			shft_reg_ >>= 8;
			shft_reg_ |= (v << 8);
		} break;
	}
}

void Machine::key_down(KeyPress k)
{
	switch (k)
	{
		case KeyPress::left:
			inp1_ |= 0x20; // bit 5
			break;
		case KeyPress::right:
			inp1_ |= 0x40; // bit 6;
			break;
		case KeyPress::up:
			inp1_ |= 0x60; // bit 7
			break;
		case KeyPress::down:
			inp1_ |= 0x80; // bit 8;
			break;
	}
}

void Machine::key_up(KeyPress k)
{
	switch (k)
	{
		case KeyPress::left:
			inp1_ &= 0xEF; // clear bit 5
			break;
		case KeyPress::right:
			inp1_ &= 0xDF; // clear bit 6;
			break;
		case KeyPress::up:
			inp1_ &= 0xBF; // clear bit 7
			break;
		case KeyPress::down:
			inp1_ &= 0x7F; // clear bit 8;
			break;
	}
}