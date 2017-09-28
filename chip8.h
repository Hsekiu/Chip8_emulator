#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

class chip8
{
public:
	chip8();
	~chip8();

	void init();
	bool loadGame(std::string game);
	bool cycle();

	std::string hexString(int a);
	const unsigned short start = 0x200;
	unsigned short screen[32][64];

	bool drawFlag;

private:
	int opcode;
	unsigned char instruction;
	unsigned char memory[4096];
	unsigned char V[16];

	unsigned short I;
	unsigned short pc;
	unsigned short stack[16];
	unsigned short stackPointer;

	char x;
	char y;
	char kk;
	short nnn;
	char n;
};

