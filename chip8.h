#pragma once
class chip8
{
public:
	chip8();
	~chip8();

private:
	unsigned short opcode;
	unsigned char memory[4096];
	unsigned char V[16];

	unsigned short I;
	unsigned short pc;
};

