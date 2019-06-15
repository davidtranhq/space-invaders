#include <iostream>
#include <fstream>
#include <string>

#include "cpu.hpp"
#include "machine.hpp"

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		throw;
	}
	std::string game;
	std::cout << "Enter the path of a ROM to load.\n";
	std::cin >> game;
	space_invaders::Machine cabinet {};
	cabinet.load_program(game, 0x00);
	cabinet.run();
	SDL_Quit();
	return 0;
}