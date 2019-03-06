#include <Windows.h>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <SDL_mixer\SDL_mixer.h>

#include "chip8.h"

#include <sstream>
#include <iostream>
#include <bitset>
#include <algorithm>

#include <string>

namespace Emulate
{
	
	class Emulation
	{
	public:
		Emulation();
		~Emulation();

		const int DISPLAY_WIDTH = 640;
		const int DISPLAY_HEIGHT = 320;
		float xs;
		float ys;
		float heights;
		float widths;

		bool newGameLoaded;

		// START starts the emulator and PAUSE pauses it.
		// STOP handles program and emulator end states.
		// FINISH resets the emulator.
		enum class EmulationState { START, PAUSE, STOP, FINISH };
		EmulationState _emulationState;

		SDL_Window *_window;
		SDL_GLContext glContext;

		char* title = "Chip8 interpreter";
		chip8 chip;

		bool step;

		std::string gameName;
		char* fileDir;

		Mix_Chunk *beep;

		bool loadGame();
		void mainLoop();
		void emulationLoop();
		void processInput();
		void drawScreen();
		bool initializeChip(std::string game);
		void cleanUp();
		void drawSquare(float height, float width, float x, float y, float color);
		bool loadAssets();
		bool initializeSDL();

	private:

	};
}