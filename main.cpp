#include <Windows.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <iostream>
using namespace std;

const int DISPLAY_WIDTH = 640;
const int DISPLAY_HEIGHT = 320;

enum class EmulationState { START, PAUSE, STOP };
EmulationState _emulationState;

SDL_Window *_window;
SDL_GLContext glContext;

char* title = "Chip8 emulator by Petr Krakora";

//Forward declarations
void emulationLoop();
bool initializeSDL();
void processInput();
void drawScreen();

int main(int argc, char** argv) {

	initializeSDL();

	_emulationState = EmulationState::START;
	emulationLoop();

	return 0;
}

void emulationLoop() {
	while (_emulationState == EmulationState::START) {
		processInput();
		drawScreen();
	}
}

bool initializeSDL() {
	SDL_Init(SDL_INIT_EVERYTHING);
	_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
	//Save openGl states
	glContext = SDL_GL_CreateContext(_window);
	//Create a double buffered window to prevent artifacts
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//Set screen to black (red, green, blue, alpha)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

void drawScreen() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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