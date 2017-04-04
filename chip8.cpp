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
	opcode = "";

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

	opcode = hexString((int)memory[start + pc]) + hexString((int)memory[start + pc + 1]);

	instruction = opcode[0];
	//std::cout << "opcode is: " << opcode << std::endl;

	switch (instruction) {
		case '0': //CLS
			std::cout << "clear screen" << std::endl;
			memset(screen, 0, sizeof screen);
			pc += 2;
			break;
		case 'a': //LD I, addr
			I = std::stoi(opcode.substr(1, 3), 0, 16);
			std::cout << "I is " << I << std::endl;
			pc += 2;
			break;

	}

	return true;
}