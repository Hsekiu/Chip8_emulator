#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>

class chip8
{
public:
	chip8();
	~chip8();

	void init();
	bool loadGame(std::string game);
	bool cycle();

	std::string hexString(int a);
	const unsigned short EMU_START = 0x200;
	const unsigned short FONT_START = 0x000;
	bool screen[32][64];
	void printData();
	void updateTimers();

	unsigned char keys[16];

	bool drawFlag;
	bool soundFlag;

private:
	int opcode;
	unsigned char instruction;
	unsigned char memory[4096];
	unsigned char V[16];

	unsigned short I;
	unsigned short pc;
	unsigned short stack[16];
	unsigned short stackPointer;

	unsigned short delayTimer;
	unsigned short soundTimer;

	unsigned char x;
	unsigned char y;
	unsigned char kk;
	unsigned short nnn;
	unsigned char n;
};

