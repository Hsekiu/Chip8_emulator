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
	pc = start;
	opcode = 0;

	memset(memory, 0, sizeof memory);
	memset(stack, 0, sizeof stack);
	memset(screen, 0, sizeof screen);

	srand(time(NULL));
}

bool chip8::loadGame(std::string game) {
	std::ifstream file(game, std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::streampos size = file.tellg();
		file.seekg(0, std::ios::beg);
		file.read((char*)&memory[start], size);

		return true;
	}

	return false;
}

bool chip8::cycle() {
	opcode = (memory[pc] << 8) + memory[pc + 1];

	std::cout << "Opcode is: " << (hexString)(opcode & 0xFFFF) << std::endl;

	//A 4-bit value, the lower 4 bits of the high byte of the instruction
	x = (opcode & 0x0F00) >> 8;
	//A 4-bit value, the upper 4 bits of the low byte of the instruction
	y = (opcode & 0x00F0) >> 4;
	//An 8-bit value, the lowest 8 bits of the instruction
	kk = opcode & 0x00FF; //byte
	//A 12-bit value, the lowest 12 bits of the instruction
	nnn = opcode & 0x0FFF; //addr
	//A 4-bit value, the lowest 4 bits of the instruction
	n = (opcode & 0x000F); //nibble
	
	switch (opcode & 0xF000) {
		case 0x0000: //SYS addr
		{
			switch (opcode & 0x00FF) {

				case 0x00E0: //00E0 - CLS
				{
					std::cout << " - Clear the display." << std::endl;
					memset(screen, 0, sizeof screen);
					pc += 2;
				}
				break;

				case 0x00EE: //00EE - RET
				{
					std::cout << " - Return from a subroutine" << std::endl;
					pc += 2;
				}
				break;

			default:
				std::cout << " - Opcode not implemented in 0x0000" << std::endl;
				pc += 2;
			}
		}
		break;

		case 0x1000: //1nnn - JP addr
		{
			std::cout << " - Jump to address " << (hexString)(start + nnn + 0x100) << std::endl;
			pc = start + nnn + 0x100;
		}
		break;

		case 0x2000: //2nnn - CALL addr
		{
			std::cout << " - Call subroutine at " << (hexString)(start + nnn + 0x100) << std::endl;
			stack[stackPointer] = pc;
			stackPointer++;
			pc = start + nnn + 0x100;
		}
		break;

		case 0x3000: //3xkk - SE Vx, byte
		{
			std::cout << " - Skip next instruction if V[" << (int)x << "] = " << (hexString)(nnn) << std::endl;
			if ((V[x]) == nnn) {
				pc += 2;
			}
			pc += 2;
		}
		break;

		case 0x4000: //4xkk - SNE Vx, byte
		{
			std::cout << " - Skip next instruction if V[" << (int)x << "] != " << (hexString)(nnn) << std::endl;
			if ((V[x]) != nnn) {
				pc += 2;
			}
		}
		break;

		case 0x5000: //SE Vx, Vy
		{
			std::cout << " - Skip next instruction if V[" << (int)x << "] = " << (hexString)(y) << std::endl;
			if ((V[x]) == y) {
				pc += 2;
			}
		}
		break;

		case 0x6000: //6xkk - LD Vx, byte
		{
			std::cout << " - Set V[" << (int)x << "] = " << (hexString)(kk) << std::endl;
			V[x] = kk;
			pc += 2;
		}
		break;

		case 0x7000: //7xkk - ADD Vx, byte
		{
			std::cout << " - Set " << (int)V[x] << " + " << (int)kk << " = " << ((int)V[x] + (int)kk) << std::endl;
			V[x] += kk;
			pc += 2;
		}
		break;

		case 0x8000:
		{
			switch (opcode & 0x000F) {

			case 0x0000: //LD Vx, Vy
			{
				std::cout << " - Set V[x] = " << "V[" << (int)y << "]" << std::endl;
				V[x] = V[y];
				pc += 2;
			}
			break;

			case 0x0001: //OR Vx, Vy
			{
				std::cout << " - Set V[x] |= " << "V[" << (int)y << "]" << std::endl;
				V[x] |= V[y];
				pc += 2;
			}
			break;

			case 0x0002: //AND Vx, Vy
			{
				std::cout << " - Set V[x] &= " << "V[" << (int)y << "]" << std::endl;
				V[x] &= V[y];
				pc += 2;
			}
			break;

			case 0x0003: //XOR Vx, Vy
			{
				std::cout << " - Set V[x] ^= " << "V[" << (int)y << "]" << std::endl;
				V[x] ^= V[y];
				pc += 2;
			}
			break;

			case 0x0004: //ADD Vx, Vy
			{
				std::cout << " - Set V[x] += " << "V[" << (int)y << "]" << std::endl;
				V[x] += V[y];

				if (((int)V[x] + (int)V[y]) > 255) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}

				pc += 2;
			}
			break;

			case 0x0005: //SUB Vx, Vy
			{
				std::cout << " - Set V[x] -= " << "V[" << (int)y << "]" << std::endl;
				V[x] -= V[y];

				if (V[y] > V[x]) {
					V[0xF] = 1;
				} else {
					V[0xF] = 0;
				}

				pc += 2;
			}
			break;

			case 0x0006: //SHR Vx {, Vy}
			{
				if ((V[x] >> 1) == 1) {
					V[0xF] = 1;
				} else {
					V[0xF] = 0;
				}

				V[x] /= 2;

				pc += 2;
			}
			break;

			case 0x0007: //SUBN Vx, Vy
			{
				if (V[y] > V[x]) {
					V[0xF] = 1;
				} else {
					V[0xF] = 0;
				}

				V[x] -= V[y];

				pc += 2;
			}
			break;

			case 0x000E: //SHL Vx {, Vy}
			{
				if ((V[x] << 1)  == 1) {
					V[0xF] = 1;
				} else {
					V[0xF] = 0;
				}

				V[x] *= 2;

				pc += 2;
			}
			break;

			default:
				std::cout << " - Opcode not implemented in 0x8000" << std::endl;
				pc += 2;
			}
		}
		break;

		case 0x900: //SNE Vx, Vy
		{
			std::cout << " - Skip next instruction if V[" << (int)x << "] != " << "V[" << (int)y << "]" << std::endl;
			if (V[x] != V[y]) {
				pc += 2;
			}
			pc += 2;
		}
		break;

		case 0xA000: //Annn - LD I, addr
		{
			std::cout << " - Set I = " << (hexString)(short(opcode & 0x0FFF)) << std::endl;
			I = short(opcode & 0x0FFF);
			pc += 2;
		}
		break;

		case 0XB000: //JP V0, addr
		{
			std::cout << " - Jump to location " << ((int)(nnn) + V[0]) << std::endl;
			pc = nnn + V[0];
		}

		case 0xC000: //Cxkk - RND Vx, byte
		{
			int random = rand() % 255;
			std::cout << " - Set V[" << (int)x << "] to random number " << (int)((kk) & random) << std::endl;
			V[x] = kk & random;
			pc += 2;
		}
		break;

		case 0xD000: //Dxyn - DRW Vx, Vy, nibble
		{
			char line = 0;

			std::cout << " - Draw at (" << (int)V[V[x]] << ", " << (int)V[V[y]] << ")." << std::endl;

			V[0xF] = 0;

			for (int i = 0; i < n; i++) {
				line = memory[I + i];

				for (int j = 0; j < 8; j++) {

					if (((line & (0x80 >> j)) != 0)) {

						if (screen[V[y] + i][V[x] + j] == 1) {
							V[0xF] = 1;
						}

					screen[V[y] + i][V[x] + j] ^= 1;
					}
				}
			}

			drawFlag = true;

			pc += 2;
		}
		break;

		case 0xF000:
		{
			switch (opcode & 0x00FF) {

			break;

			case 0x001E: //Fx1E - ADD I, Vx
			{
				std::cout << " - Set I += V[" << (int)x << "], = " << (int)(I + V[x]) << std::endl;
				I += V[x];
				pc += 2;
			}
			break;

			default:
				std::cout << " - Opcode not implemented in 0xF000" << std::endl;
				pc += 2;
			}
		}
		break;

		default:
			std::cout << " - Opcode not implemented" << std::endl;
	}

	return true;
}