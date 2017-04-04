#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class chip8
{
public:
	chip8();
	~chip8();

	bool loadGame(std::string game);
	bool cycle();

private:
	unsigned short opcode;
	unsigned char memory[4096];
	unsigned char V[16];

	unsigned short I;
	unsigned short pc;
};

