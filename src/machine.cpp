#include "machine.hpp"

#include <fstream>
#include <bitset>

namespace space_invaders
{
	
Machine::Machine()
	: cpu_
	(
		memory_,
		[this](uint8_t o) { return this->in(o); },
		[this](uint8_t p, uint8_t val) { this->out(p, val); }
	),
	window_ {SDL_CreateWindow("Space Invaders!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE)},
	disp_ {SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0)},
	sounds_
	{
		Wav("audio/ufo_low"),
		Wav("audio/shoot"),
		Wav("audio/explosion"),
		Wav("audio/invader_killed"),
		Wav("audio/fleet1"),
		Wav("audio/fleet2"),
		Wav("audio/fleet3"),
		Wav("audio/fleet4"),
		Wav("audio/ufo_high")
	}
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
	long cyc_ran {0};
	long cyc_start {0};
	long cyc_finish {0};
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
		if ((SDL_GetTicks() - last_tic) >= tic)
		{
			last_tic = SDL_GetTicks();
			execute_cpu(cycles_per_tic / 2);
			cpu_.interrupt(0xCF);
			execute_cpu(cycles_per_tic / 2);
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
					done_ = true;
				else if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
						case SDLK_t:
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
			update_screen();
			cpu_.interrupt(0xD7);
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
				if (memory_[i] & 1 << j)
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

bool Machine::load_program(const std::string &in, uint16_t off)
{
	std::ifstream f {in, std::ios::binary};
	if (!f.good())
		return false;
	std::copy
	(
		std::istreambuf_iterator<char>(f),
		std::istreambuf_iterator<char>(),
		memory_.begin() + off
	);
	return true;
}

uint8_t Machine::in(uint8_t port)
{
	uint8_t a;
	switch (port)
	{
		case 1:
			a = inp1_;
			break;
		case 2:
			a = inp2_;
			break;
		case 3:
		{
			uint16_t v = (shift1<<8) | shift0;    
            a = ((v >> (8-shift_offset)) & 0xff);    
		}
		break;
	}
	return a;
}

void Machine::play_sound()
{
	if (sound1_ != last_sound1_) // bit changed
	{
		if ( (sound1_ & 0x2) && !(last_sound1_ & 0x2) )
			sounds_[1].play();
        if ( (sound1_ & 0x4) && !(last_sound1_ & 0x4) )
            sounds_[2].play();
        if ( (sound1_ & 0x8) && !(last_sound1_ & 0x8) )
			sounds_[3].play();
		last_sound1_ = sound1_;
	}
	if (sound2_ != last_sound2_)
	{
		if ( (sound2_ & 0x1) && !(last_sound2_ & 0x1) )
			sounds_[4].play();
		if ( (sound2_ & 0x2) && !(last_sound2_ & 0x2) )
			sounds_[5].play();
		if ( (sound2_ & 0x4) && !(last_sound2_ & 0x4) )
			sounds_[6].play();
		if ( (sound2_ & 0x8) && !(last_sound2_ & 0x8) )
			sounds_[7].play();
		if ( (sound2_ & 0x10) && !(last_sound2_ & 0x10) )
			sounds_[8].play();
		last_sound2_ = sound2_;
	}
}

void Machine::out(uint8_t port, uint8_t val)
{
	switch (port)
	{
		case 2:
			shift_offset = val & 0x7;    
			break;
		case 3: // play sound
			sound1_ = val;
			break;
		case 4:
		{
			shift0 = shift1;    
            shift1 = val;    
		} break;
		case 5:
			sound2_ = val;
			break;
	}
	play_sound();
}

void Machine::key_down(SDL_Keycode k)
{
	switch (k)
	{
		case SDLK_c: // insert coin
			inp1_ |= 1;
			break;
		case SDLK_s: // P1 Start
			inp1_ |= 1 << 2;
			break;
		case SDLK_w: // P1 Shoot
			inp1_ |= 1 << 4;
			break;
		case SDLK_a: // P1 left
			inp1_ |= 1 << 5;
			break;
		case SDLK_d: // P1 right
			inp1_ |= 1 << 6;
			break;
		case SDLK_LEFT: // P2 left
			inp2_ |= 1 << 5; 
			break;
		case SDLK_RIGHT: // P2 right
			inp2_ |= 1 << 6;
			break;
		case SDLK_RETURN: // P2 start
			inp1_ |= 1 << 1;
			break;
		case SDLK_UP: // P2 shoot
			inp2_ |= 1 << 4;
			break;
	}
}

void Machine::key_up(SDL_Keycode k)
{
	switch (k)
	{
		case SDLK_c: // insert coin
			inp1_ &= ~1;
			break;
		case SDLK_s: // P1 Start
			inp1_ &= ~(1 << 2);
			break;
		case SDLK_w: // P1 Shoot
			inp1_ &= ~(1 << 4);
			break;
		case SDLK_a: // P1 left
			inp1_ &= ~(1 << 5);
			break;
		case SDLK_d: // P1 right
			inp1_ &= ~(1 << 6);
			break;
		case SDLK_LEFT: // P2 left
			inp2_ &= ~(1 << 5); 
			break;
		case SDLK_RIGHT: // P2 right
			inp2_ &= ~(1 << 6);
			break;
		case SDLK_RETURN: // P2 start
			inp1_ &= ~(1 << 1);
			break;
		case SDLK_UP: // P2 shoot
			inp2_ &= ~(1 << 4);
			break;
	}
}

}