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
		case 0x0000: //SYS addr

			switch (opcode & 0x00FF) {

				case 0x00E0: //CLS
					std::cout << "CLS called" << std::endl;
					memset(screen, 0, sizeof screen);
					pc += 2;
					break;

				case 0x00EE: //RET
					std::cout << "RET called" << std::endl;
					pc += 2;
					break;

				default:
					std::cout << "Opcode not implemented in 0x0000" << std::endl;
					pc += 2;
			}
			pc += 2;
			break;

		case 0x1000: //JP addr
			pc = opcode & 0x0FFF;
			pc += 2;
			break;

		case 0x3000: //SE Vx, byte
			std::cout << "SE Vx, byte called" << std::endl;
			if ((V[(opcode & 0x0F00) >> 8]) == (opcode & 0x0FFF)) {
				pc += 2;
			}
			pc += 2;
			break;

		case 0x6000: //LD Vx, byte
			std::cout << "LD Vx, byte called" << std::endl;
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
			break;

		case 0x7000: //ADD Vx, byte
			std::cout << "Add Vx, byte called" << std::endl;
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
			break;

		case 0xA000: //LD I, addr
			std::cout << "LD I, addr" << std::endl;
			I = opcode & 0x0FFF;
			pc += 2;
			break;

		default:
			std::cout << "Opcode not implemented" << std::endl;
	}

	return true;
}