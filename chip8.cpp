#include "chip8.h"

//Built in fontset 5 bytes long, 8x5 pixels.
unsigned char fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F 
};

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
	//Reset registers.
	pc = EMU_START;
	opcode = 0;
	I = 0;
	stackPointer = 0;

	//Reset timers.
	delayTimer = 0;
	soundTimer = 0;

	//Init flags.
	soundFlag = false;
	drawFlag = false;

	//Reset array registers.
	memset(memory, 0, sizeof(memory) / sizeof(char));
	memset(stack, 0, sizeof(stack) / sizeof(short));
	memset(V, 0, sizeof(V) / sizeof(char));
	memset(screen, 0, sizeof screen);
	memset(keys, 0, sizeof(keys) / sizeof(int));

	//Load fontset at memory location from 0x0000 to 0x1FF0.
	for (int i = 0; i < (sizeof(fontset) / sizeof(unsigned char)); i++) {
		memory[FONT_START + i] = fontset[i];
	}

	srand(time(NULL));
}

bool chip8::loadGame(std::string game) {
	std::ifstream file(game, std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::streampos size = file.tellg();
		file.seekg(0, std::ios::beg);
		file.read((char*)&memory[EMU_START], size);

		return true;
	}

	return false;
}

void chip8::updateTimers() {

	if (delayTimer > 0) {
		delayTimer--;
	}

	if (soundTimer > 0) {
		soundTimer--;
		//Play sound.
		soundFlag = true;
	}
}

void chip8::printData() {

	std::cout << std::endl;
	std::cout << "     V   S   " << std::endl;
	std::cout << "   _________" << std::endl;
	for (int i = 0; i < sizeof(V); i++) {
		if (i <= 9) {
			std::cout << i << "   " << (hexString)(V[i]) << " | " << (hexString)(stack[i]) << std::endl;
		} else {
			std::cout << i << "  " << (hexString)(V[i]) << " | " << (hexString)(stack[i]) << std::endl;
		}
	}
	std::cout << "   _________" << std::endl;
	std::cout << std::endl;
	std::cout << "PC " << (hexString)(pc) << std::endl;
	std::cout << "SP " << (hexString)(stackPointer) << std::endl;
	std::cout << "I  " << (hexString)(I) << std::endl;
	std::cout << std::endl;

	/*for (int i = 0; i < sizeof(keys) / sizeof(int); i++) {
		std::cout << "Key " << i << " is: " << keys[i] << std::endl;
	}*/
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
					stackPointer--;
					pc = stack[stackPointer];
				}
				break;

			default:
				std::cout << " - Opcode not implemented in 0x0000" << std::endl;
			}
		}
		break;

		case 0x1000: //1nnn - JP addr
		{
			std::cout << " - Jump to address " << (hexString)(nnn) << std::endl;
			pc = nnn;
		}
		break;

		case 0x2000: //2nnn - CALL addr
		{
			std::cout << " - Call subroutine at " << (hexString)(nnn) << std::endl;
			stack[stackPointer] = pc + 2;
			++stackPointer;
			pc = nnn;
		}
		break;

		case 0x3000: //3xkk - SE Vx, byte
		{
			std::cout << " - Skip next instruction if V[" << (hexString)(x) << "] = " << (hexString)(kk) << std::endl;
			if ((V[x]) == kk) {
				pc += 2;
			}
			pc += 2;
		}
		break;

		case 0x4000: //4xkk - SNE Vx, byte
		{
			std::cout << " - Skip next instruction if V[" << (hexString)(x) << "] != " << (hexString)(kk) << std::endl;
			if ((V[x]) != kk) {
				pc += 2;
			}
			pc += 2;
		}
		break;

		case 0x5000: //SE Vx, Vy
		{
			std::cout << " - Skip next instruction if V[" << (hexString)(x) << "] = " << (hexString)(y) << std::endl;
			if ((V[x]) == V[y]) {
				pc += 2;
			}
			pc += 2;
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

				if ((int(V[x]) + int(V[y])) > 255) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}

				V[x] += V[y];

				pc += 2;
			}
			break;

			case 0x0005: //SUB Vx, Vy
			{
				std::cout << " - Set V[x] -= " << "V[" << (int)y << "]" << std::endl;

				if (V[x] >= V[y]) {
					V[0xF] = 1;
				} else {
					V[0xF] = 0;
				}

				V[x] -= V[y];

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
			}
		}
		break;

		case 0x9000: //SNE Vx, Vy
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
			std::cout << "Random is: " << random << std::endl;
			std::cout << " - Set V[" << (int)x << "] to random number " << (int)((kk) & random) << std::endl;
			V[x] = kk & (char)random;
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

		case 0xE000:
		{
			switch (opcode & 0x00FF) {

			case 0x009E: //SKP Vx
			{
				std::cout << " - Skip next instruction if " << (hexString)(keys[V[x]]) << " == 1." << std::endl;
				if (keys[V[x]] == 1) {
					pc += 2;
				}

				pc += 2;
			}
			break;

			case 0x00A1: //SKNP Vx
			{
				std::cout << " - Skip next instruction if " << (hexString)(keys[V[x]]) << " != 1." << std::endl;
				if (keys[V[x]] != 1) {
					pc += 2;
				}

				pc += 2;
			}
			break;

			default:
				std::cout << " - Opcode not implemented in 0xE000" << std::endl;
			}
		}
		break;

		case 0xF000:
		{
			switch (opcode & 0x00FF) {

			case 0x0007:
			{
				std::cout << " - Set " << "V[" << (int)x << "] to Delay timer = " << delayTimer << std::endl;
				V[x] = (char)(delayTimer);
				pc += 2;
			}
			break;

			case 0x000A: //LD Vx, K
			{
				std::cout << " - Wait for a key press, store the value of the key in V[" << (int)x << "]." << std::endl;
				for (int i = 0; i < 16; i++) {
					if (keys[i] == 1) {
						V[x] = (char)i;
						pc += 2;
						break;
					}
				}

			}
			break;

			case 0x0015: //LD DT, Vx
			{
				std::cout << " - Set delay timer = " << "V[" << (int)x << "]." << std::endl;
				delayTimer = V[x];
				pc += 2;
			}
			break;

			case 0x0018: //LD ST, Vx
			{
				std::cout << " - Set sound timer = " << "V[" << (int)x << "]." << std::endl;
				soundTimer = V[x];
				pc += 2;
			}
			break;

			case 0x001E: //Fx1E - ADD I, Vx
			{
				std::cout << " - Set I += V[" << (int)x << "], = " << (int)(I + V[x]) << std::endl;
				I += V[x];
				pc += 2;
			}
			break;

			case 0x0029: //LD F, Vx
			{
				I = V[x] * 5;
				pc += 2;
			}
			break;

			case 0x0033: //LD B, Vx
			{
				std::cout << " - Storing BCD representation of Vx in memory" << std::endl;

				int n = V[x];

				short count = 0;
				for (int i = 2; i >= 0; i--) {
					memory[I + count] = (n - (n % (int)pow(10, i))) / (int)pow(10, i);
					std::cout << i << " Number is : " <<(int) memory[I + count] << std::endl;
					n -= memory[I + count] * (int)pow(10, i);
					count++;
				}

				pc += 2;
			}
			break;

			case 0x0055: //Fx55 - LD [I], Vx
			{
				std::cout << " - Stored V[0] through V[ " << (int)x << "] in memory starting at location " << (int)I << std::endl;

				for (int i = 0; i <= x; i++) {
					memory[I + i] = V[i];
				}

				I++;

				pc += 2;
			}
			break;

			case 0x0065: //Fx65 - LD Vx, [I]
			{
				std::cout << " - Loaded Registers V[0] through V[ " << (int)x << "] from memory starting at location " << (int)I << std::endl;

				for (int i = 0; i <= x; i++) {
					V[i] = memory[I + i];
				}

				I++;

				pc += 2;
			}
			break;

			default:
				std::cout << " - Opcode not implemented in 0xF000" << std::endl;
			}
		}
		break;

		default:
			std::cout << " - Opcode not implemented" << std::endl;
	}

	updateTimers();

	return true;
}