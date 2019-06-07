#include "machine.hpp"

namespace dav
{
	
namespace i8080
{
	
using namespace std::chrono;
using Clock = high_resolution_clock;
	
Machine::Machine()
	: cpu_
	( 
		[this](uint8_t o) { return this->in(o); },
		[this](uint8_t p, uint8_t val) { this->out(p, val); }
	),
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

void Machine::execute_cpu(long cyc)
{
	int cyc_ran {0};
	int cyc_start {0};
	int cyc_finish {0};
	while (cyc_ran < cyc)
	{
		cyc_start = cpu_.cycles();
		cpu_.emulate_op();
		cyc_finish = cpu_.cycles();
		cyc_ran += (cyc_finish - cyc_start);
	}
}

void Machine::run()
{
	SDL_Event e;
	uint32_t last_tic = SDL_GetTicks();
	constexpr double tic = 1000.0 / 60.0; // ms per tic
	constexpr int cycles_per_ms = 2000; // 2 Mhz
	constexpr double cycles_per_tic = cycles_per_ms * tic;
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
					default:
						key_down(e.key.keysym.sym);
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				switch (e.key.keysym.sym)
				{
					default:
						key_up(e.key.keysym.sym);
				}
			}
		}
		if ((SDL_GetTicks() - last_tic) >= tic)
		{
			last_tic = SDL_GetTicks();
			execute_cpu(cycles_per_tic / 2);
			cpu_.interrupt(0xCF);
			execute_cpu(cycles_per_tic / 2);
			update_screen();
			cpu_.interrupt(0xD7);
			last_tic = SDL_GetTicks();
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
				if (cpu_.memory()[i] & 1 << j)
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

void Machine::key_down(SDL_Keycode k)
{
	switch (k)
	{
		case SDLK_c:
			inp1_ |= 0x1; // coin
			std::cout << "inserted coin";
			break;
		case SDLK_LEFT:
			inp1_ |= 0x20; // bit 5
			break;
		case SDLK_RIGHT:
			inp1_ |= 0x40; // bit 6;
			break;
		case SDLK_UP:
			inp1_ |= 0x60; // bit 7
			break;
		case SDLK_DOWN:
			inp1_ |= 0x80; // bit 8;
			break;
	}
}

void Machine::key_up(SDL_Keycode k)
{
	switch (k)
	{
		case SDLK_c:
			inp1_ |= ~0x1;
			break;
		case SDLK_LEFT:
			inp1_ &= ~0x20; // clear bit 5
			break;
		case SDLK_RIGHT:
			inp1_ &= ~0x40; // clear bit 6;
			break;
		case SDLK_UP:
			inp1_ &= ~0x60; // clear bit 7
			break;
		case SDLK_DOWN:
			inp1_ &= ~0x80; // clear bit 8;
			break;
	}
}

}

}