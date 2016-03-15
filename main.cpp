#include <SDL/SDL.h>

#include <iostream>
using namespace std;

const int DISPLAY_WIDTH = 640;
const int DISPLAY_HEIGHT = 320;

enum class EmulationState { START, PAUSE, STOP };
EmulationState _emulationState;

SDL_Window *window;

char* title = "Chip8 emulator by Petr Krakora";

//Forward declarations
void emulationLoop();
bool initializeSDL();
void processInput();

int main(int argc, char** argv) {

	initializeSDL();

	_emulationState = EmulationState::START;
	emulationLoop();

	cout << "shouildn't be here" << endl;
	return 0;
}

void emulationLoop() {
	while (_emulationState == EmulationState::START) {
		processInput();
	}
}

bool initializeSDL() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
	return true;
}

void processInput() {
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			_emulationState = EmulationState::STOP;
			break;
		}
	}
}