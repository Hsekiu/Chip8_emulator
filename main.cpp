#include "Emulation.h"

using namespace Emulate;

Emulation emulator;

int main(int argc, char** argv) {

	//Close application if initialization failure.
	if (!(emulator.initializeSDL())) {
		emulator.cleanUp();
	}

	emulator.beep = NULL;

	//Load music.
	emulator.loadAssets();

	emulator._emulationState = Emulation::EmulationState::PAUSE;

	//Set up "pixel" dimensions.
	emulator.widths = 1.0 / 64;
	emulator.heights = 1.0 / 32;

	//Emulation instruction stepping
	emulator.step = false;

	emulator.newGameLoaded = false;

	//Main loop of program.
	emulator.mainLoop();

	emulator.cleanUp();

	return 0;
}