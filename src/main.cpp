#include <iostream>
#include <fstream>

#include "cpu.hpp"
#include "machine.hpp"

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	std::string game;
	std::cout << "Enter the path of a ROM to load.\n";
	std::cin >> game;
	i8080::Machine arcade_cabinet {};
	arcade_cabinet.load_program(game, 0x00);
	arcade_cabinet.run();
	SDL_Quit();
	return 0;
}