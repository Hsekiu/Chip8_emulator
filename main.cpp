#include <Windows.h>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include "chip8.h"

#include <sstream>
#include <iostream>
#include <bitset>
using namespace std;

const int DISPLAY_WIDTH = 640;
const int DISPLAY_HEIGHT = 320;
float xs = 0;
float ys = 0;
float heights;
float widths;

enum class EmulationState { START, PAUSE, STOP };
EmulationState _emulationState;

SDL_Window *_window;
SDL_GLContext glContext;

char* title = "Chip8 emulator by Petr Krakora";
chip8 chip;

//Forward declarations
void emulationLoop();
bool initializeChip(string game);
bool initializeSDL();
void processInput();
void drawScreen();

int main(int argc, char** argv) {

	initializeSDL();

	_emulationState = EmulationState::START;
	//std::cout << "Argument 1 is: " << argv[1] << std::endl;
	//initializeChip(argv[1]);

	widths = 1.0 / 64;
	heights = 1.0 / 32;

	emulationLoop();

	return 0;
}

bool initializeChip(string game) {
	chip.loadGame(game);
	return 1;
}

void emulationLoop() {
	chip.loadGame("Maze [David Winter, 199x]");

	while (_emulationState != EmulationState::STOP) {
		processInput();

		if (_emulationState != EmulationState::PAUSE) {
			chip.cycle();
		}

		drawScreen();
	}
}

bool initializeSDL() {
	SDL_Init(SDL_INIT_EVERYTHING);
	_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
	//Save openGl states
	glContext = SDL_GL_CreateContext(_window);
	GLenum errorGl = glewInit();
	if (errorGl != GLEW_OK)
		cout << "Couldnt Initalize Glew!" << endl;
	//Create a double buffered window to prevent artifacts
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//Set screen to black (red, green, blue, alpha)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

void drawSquare(float height, float width, float x, float y) {

	GLuint vbo = 0;
	
	if (vbo == 0) {
		glGenBuffers(1, &vbo);
	}

	x -= 1.0;
	y += 1.0;
	
	GLfloat vertData[] = {
		x, y,
		x + width, y,
		x, y - height,
		//second triangle
		x, y - height,
		x + width, y,
		x + width, y - height
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertData), vertData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vbo);
	
}

void drawScreen() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int j = 0; j < 32; j++) {

		for (int i = 0; i < 64; i++) {
			if (chip.screen[j][i] == 1) {
				drawSquare(heights * 2, widths * 2, xs, ys);
			}
				xs += widths * 2;
		}
		xs = 0;
		ys -= heights * 2;
	}
	
	xs = 0;
	ys = 0;

	SDL_GL_SwapWindow(_window);
}

void processInput() {
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt) != 0) {
		if (evnt.type == SDL_QUIT) {
			_emulationState = EmulationState::STOP;
		}
		else if (evnt.type == SDL_KEYDOWN) {
			switch (evnt.key.keysym.sym) {
			case SDLK_1:
				cout << "Pressed 1" << endl;
				break;
			case SDLK_2:
				cout << "Pressed 2" << endl;
				break;
			case SDLK_3:
				cout << "Pressed 3" << endl;
				break;
			case SDLK_4:
				cout << "Pressed 4" << endl;
				break;
			case SDLK_SPACE:
				cout << "Pressed Space" << endl;
				if (_emulationState == EmulationState::PAUSE) {
					_emulationState = EmulationState::START;
				}
				else {
					_emulationState = EmulationState::PAUSE;
				}
			}
		}
		else if (evnt.type == SDL_KEYUP) {
			switch (evnt.key.keysym.sym) {
			case SDLK_1:
				cout << "Let go of 1" << endl;
				break;
			case SDLK_2:
				cout << "Let go of 2" << endl;
				break;
			case SDLK_3:
				cout << "Let go of 3" << endl;
				break;
			case SDLK_4:
				cout << "Let go of 4" << endl;
				break;
			}
		}
	}
}