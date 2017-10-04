#include <Windows.h>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include "chip8.h"

#include <sstream>
#include <iostream>
#include <bitset>
#include <algorithm>

using namespace std;

const int DISPLAY_WIDTH = 640;
const int DISPLAY_HEIGHT = 320;
float xs = 0;
float ys = 0;
float heights;
float widths;

// START starts the emulator and PAUSE pauses it.
// STOP handles program and emulator end states.
// FINISH resets the emulator.
enum class EmulationState { START, PAUSE, STOP, FINISH };
EmulationState _emulationState;
 
SDL_Window *_window;
SDL_GLContext glContext;

char* title = "Chip8 emulator by Petr Krakora";
chip8 chip;

bool step;
bool newGameLoaded;

string gameName;
char* fileDir;

//Forward declarations
void emulationLoop();
bool initializeChip(string game);
bool initializeSDL();
void processInput();
void drawScreen();
void mainLoop();
bool loadGame();

int main(int argc, char** argv) {

	initializeSDL();

	_emulationState = EmulationState::PAUSE;

	//Set up "pixel" dimensions.
	widths = 1.0 / 64;
	heights = 1.0 / 32;

	//Emulation instruction stepping
	step = false;
	
	newGameLoaded = false;

	//Main loop of program.
	mainLoop();

	return 0;
}

bool initializeChip(string game) {
	chip.init();
	return chip.loadGame(game);
}

void mainLoop() {
	while (_emulationState != EmulationState::STOP) {

		if (newGameLoaded) {
			loadGame();
			emulationLoop();
		} else {
			processInput();
		}
	}
}

//Emulation of CPU loop.
void emulationLoop() {

	while (_emulationState != EmulationState::STOP || _emulationState == EmulationState::FINISH) {

		//Reloading game.
		if (_emulationState == EmulationState::FINISH) {
			initializeChip(gameName);
			memset(chip.screen, 0, sizeof chip.screen);
			drawScreen();
			_emulationState = EmulationState::PAUSE;
		}

		//Stepping of Emulation.
		if (_emulationState != EmulationState::PAUSE || step == true) {
			chip.cycle();
			step = false;
		}

		processInput();

		//New game Loaded.
		if (newGameLoaded) {
			loadGame();
		}

		//If dragflag then screen is updated and drawn.
		if (chip.drawFlag == true) {
			drawScreen();
			chip.drawFlag = false;
		}
	}
}

bool loadGame() {
	newGameLoaded = false;

	if (chip.loadGame(gameName)) {
		_emulationState = EmulationState::START;
		initializeChip(gameName);
		std::cout << "Loaded " << gameName << std::endl;
		return true;
	}
	else {
		std::cout << "Could not read Rom file." << std::endl;
		return false;
	}

	return false;
}

//Boilerplate SDL setup.
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
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	return true;
}

void drawSquare(float height, float width, float x, float y, float color) {

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

	glColor3f(color, color, color);

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
	glClear(GL_COLOR_BUFFER_BIT);

	for (int j = 0; j < 32; j++) {

		for (int i = 0; i < 64; i++) {
			if (chip.screen[j][i] == 1) {
				//Draw white square
				drawSquare(heights * 2, widths * 2, xs, ys, 1.0f);
			}
			else {
				//Draw black square
				drawSquare(heights * 2, widths * 2, xs, ys, 0.0f);
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
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			_emulationState = EmulationState::STOP;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_1:
				cout << "Pressed 1" << endl;
				chip.keys[1] = 1;
				break;
			case SDLK_2:
				cout << "Pressed 2" << endl;
				chip.keys[2] = 1;
				break;
			case SDLK_3:
				cout << "Pressed 3" << endl;
				chip.keys[3] = 1;
				break;
			case SDLK_4:
				cout << "Pressed 4" << endl;
				chip.keys[4] = 1;
				break;
			case SDLK_SPACE:
				if (_emulationState == EmulationState::PAUSE) {
					_emulationState = EmulationState::START;
				}
				else {
					_emulationState = EmulationState::PAUSE;
				}
			}
		}
		else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
			case SDLK_1:
				cout << "Let go of 1" << endl;
				chip.keys[1] = 0;
				break;
			case SDLK_2:
				cout << "Let go of 2" << endl;
				chip.keys[2] = 0;
				break;
			case SDLK_3:
				cout << "Let go of 3" << endl;
				chip.keys[3] = 0;
				break;
			case SDLK_4:
				cout << "Let go of 4" << endl;
				chip.keys[4] = 0;
				break;
			case SDLK_LEFT:
				step = true;
				break;
			case SDLK_r:
				cout << "Reloading game" << endl;
				_emulationState = EmulationState::FINISH;
				break;
			case SDLK_UP:
				cout << "Printing data" << endl;
				chip.printData();
				break;
			}
		}
		else if (event.type == SDL_DROPFILE) {
			fileDir = event.drop.file;
			gameName = string(fileDir);
			SDL_free(fileDir);
			newGameLoaded = true;
		}
	}
}