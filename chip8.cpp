#include "chip8.h"

chip8::chip8()
{
}


chip8::~chip8()
{
}

bool chip8::loadGame(std::string game) {

	std::ifstream file(game, std::ios::in | std::ios::binary | std::ios::ate);
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);

	file.read((char*)&memory[0], size);

	return true;

}

bool chip8::cycle() {
	return true;
}