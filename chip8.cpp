#include "chip8.h"

chip8::chip8()
{
}


chip8::~chip8()
{
}

std::string chip8::hexString(int a)
{
	std::stringstream ss;
	ss << std::hex << a;
	std::string result = "";
	result = ss.str();
	if (result.length() == 1) {
		result = "0" + result;
	}
	if (result.length() > 2) {
		result.substr(1, 2);
	}
	return result;
}



void chip8::init() {
	pc = start + 2;
	opcode = 0;

	memset(memory, 0, sizeof memory);
	memset(stack, 0, sizeof stack);
	memset(screen, 0, sizeof screen);
}

bool chip8::loadGame(std::string game) {
	 
	std::ifstream file(game, std::ios::in | std::ios::binary | std::ios::ate);
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);
	file.read((char*)&memory[start], size);

	return true;

}

bool chip8::cycle() {

	opcode = (memory[pc + start] << 8) + memory[pc + start + 1];

	switch (opcode & 0xF000) {
		case 0x1000: //CLS
			std::cout << "0 called" << std::endl;
			memset(screen, 0, sizeof screen);
			pc += 2;
			break;
		case 0x6000: //LD Vx, byte
			std::cout << "6 called" << std::endl;
			//V[std::stoi(opcode.substr(1, 1), 0, 16)] = std::stoi(opcode.substr(2, 3), 0, 16);
			pc += 2;
			break;
		case 0xA000: //LD I, addr
			std::cout << "A called" << std::endl;
			//I = std::stoi(opcode.substr(1, 3), 0, 16);
			pc += 2;
			break;
		default:
			std::cout << "Opcode not implemented" << std::endl;
	}

	return true;
}