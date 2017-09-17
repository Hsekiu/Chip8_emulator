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

	std::cout << "Opcode is: " << (hexString)(opcode & 0xFFFF) << std::endl;

	switch (opcode & 0xF000) {
		case 0x0000: //SYS addr
		{
			switch (opcode & 0x00FF) {

				case 0x00E0: //CLS
				{
					std::cout << " - Clear the display." << std::endl;
					memset(screen, 0, sizeof screen);
					pc += 2;
				}
				break;

				case 0x00EE: //RET
				{
					std::cout << " - RET called" << std::endl;
					pc += 2;
				}
				break;

			default:
				std::cout << " - Opcode not implemented in 0x0000" << std::endl;
				pc += 2;
			}
		}
		break;

		case 0x1000: //JP addr
		{
			char nn = opcode & 0x0FFF;
			std::cout << " - Jump to address " << (hexString)(opcode & 0x0FFF) << std::endl;
			pc = nn;
		}
		break;

		case 0x2000: //CALL addr
		{
			std::cout << " - CALL addr called" << std::endl;
			stack[stackPointer] = pc;
			stackPointer++;
			pc = opcode & 0x0FFF;
		}
		break;

		case 0x3000: //SE Vx, byte
		{
			std::cout << " - SE Vx, byte called" << std::endl;
			if ((V[(opcode & 0x0F00) >> 8]) == (opcode & 0x0FFF)) {
				pc += 2;
			}
			pc += 2;
		}
		break;

		case 0x4000: //SNE Vx, byte
		{
			std::cout << " - SNE Vx, byte" << std::endl;
			if ((V[(opcode & 0x0F00) >> 8]) != (opcode & 0x0FFF)) {
				pc += 2;
			}
		}
		break;

		case 0x5000: //SE Vx, Vy
		{
			std::cout << " - SE Vx, Vy" << std::endl;
			if ((V[(opcode & 0x0F00) >> 8]) == ((opcode & 0x00F0) >> 4)) {
				pc += 2;
			}
		}
		break;

		case 0x6000: //LD Vx, byte
		{
			char x = (opcode & 0x0F00) >> 8;
			char kk = opcode & 0x00FF;
			std::cout << " - Set V[" << (int)x << "] = " << (hexString)(kk) << std::endl;
			V[x] = kk;
			pc += 2;
		}
		break;

		case 0x7000: //ADD Vx, byte
		{
			char x = (opcode & 0x0F00) >> 8;
			char kk = opcode & 0x00FF;
			std::cout << " - Set " << (int)V[x] << " + " << (int)kk << " = " << ((int)V[x] + (int)kk) << std::endl;
			V[x] += kk;
			pc += 2;
		}
		break;

		case 0xA000: //LD I, addr
		{
			short nn = opcode & 0x0FFF;
			std::cout << " - Set I = " << (hexString)(nn) << std::endl;
			I = nn;
			pc += 2;
		}
		break;

		case 0xC000:
		{
			char x = (opcode & 0x0F00) >> 8;
			char kk = (opcode & 0x00FF);
			int random = rand() % 255;
			std::cout << " - Set V[" << (int)x << "] to random number " << (int)((kk) & random) << std::endl;
			V[x] = kk & random;
			pc += 2;
		}
		break;

		case 0xD000: //DRW Vx, Vy, nibble
		{
			char x = V[((opcode & 0x0F00) >> 8)];
			char y = V[((opcode & 0x00F0) >> 4)];
			char n = (opcode & 0x000F);
			char line = 0;

			std::cout << " - Draw at (" << (int)V[x] << ", " << (int)V[y] << ")." << std::endl;

			V[0xF] = 0;

			for (int i = 0; i < n; i++) {
				line = memory[I + i];

				for (int j = 0; j < 8; j++) {

					if (((line & (0x80 >> j)) != 0)) {

						if (screen[y + i][x + j] == 1) {
							V[0xF] = 1;
						}

					screen[y + i][x + j] ^= 1;
					}
				}
			}

			pc += 2;
		}
		break;


		default:
			std::cout << " - Opcode not implemented" << std::endl;
	}

	return true;
}