#include "Emulation.h"

using namespace Emulate;

bool Emulate::Emulation::loadGame() {
	newGameLoaded = false;

	if (chip.loadGame(gameName)) {
		_emulationState = Emulation::EmulationState::START;
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

void Emulate::Emulation::mainLoop() {
	while (_emulationState != EmulationState::STOP) {

		if (newGameLoaded) {
			loadGame();
			emulationLoop();
		}
		else {
			processInput();
		}
	}
}

//Boilerplate SDL setup.
bool Emulate::Emulation::initializeSDL() {

	//Initialize all of SDL's subsystems.
	//Doesnt Init everything so ignore.
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cout << "Couldnt init SDL!" << std::endl;
		return false;
	}

	_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);

	//Check if there is no error with screen setup.
	if (_window == NULL) {
		std::cout << "Couldnt create Window!" << std::endl;
		return false;
	}

	//Save openGl states
	glContext = SDL_GL_CreateContext(_window);
	GLenum errorGl = glewInit();

	//Glew init check.
	if (errorGl != GLEW_OK) {
		std::cout << "Couldnt Initalize Glew!" << glewGetErrorString(errorGl) << std::endl;
		return false;
	}

	//Initialize SDL_mixer 
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		std::cout << "Couldnt Initialize SDL_mixer: " << Mix_GetError() << std::endl;
		return false;
	}

	//Create a double buffered window to prevent artifacts
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//Set screen to black (red, green, blue, alpha)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	return true;
}

bool Emulate::Emulation::loadAssets() {
	//Load the music 
	beep = Mix_LoadWAV("tone_beep.wav");

	//Lower volume to 16 out of 0 to 128.
	Mix_VolumeChunk(beep, MIX_MAX_VOLUME / 8);

	//Check if problem loading SFX.
	if (beep == NULL) {
		std::cout << "Problem loading SFX: " << Mix_GetError() << std::endl;
		return false;
	}

	return true;
}

void Emulate::Emulation::drawSquare(float height, float width, float x, float y, float color) {

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

void Emulate::Emulation::cleanUp() {
	Mix_FreeChunk(beep);
	Mix_CloseAudio();
	SDL_Quit();
}

bool Emulate::Emulation::initializeChip(std::string game) {
	chip.init();
	return chip.loadGame(game);
}

void Emulate::Emulation::drawScreen() {
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

void Emulate::Emulation::processInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			_emulationState = EmulationState::STOP;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_1: chip.keys[1] = 1; break;
			case SDLK_2: chip.keys[2] = 1; break;
			case SDLK_3: chip.keys[3] = 1; break;
			case SDLK_4: chip.keys[12] = 1; break;
			case SDLK_q: chip.keys[4] = 1; break;
			case SDLK_w: chip.keys[5] = 1; break;
			case SDLK_e: chip.keys[6] = 1; break;
			case SDLK_r: chip.keys[13] = 1; break;
			case SDLK_a: chip.keys[7] = 1; break;
			case SDLK_s: chip.keys[8] = 1; break;
			case SDLK_d: chip.keys[9] = 1; break;
			case SDLK_f: chip.keys[14] = 1; break;
			case SDLK_z: chip.keys[10] = 1; break;
			case SDLK_x: chip.keys[0] = 1; break;
			case SDLK_c: chip.keys[11] = 1; break;
			case SDLK_v: chip.keys[15] = 1; break;
			case SDLK_SPACE:
				if (_emulationState == EmulationState::PAUSE) {
					_emulationState = EmulationState::START;
				}
				else {
					_emulationState = EmulationState::PAUSE;
				} break;
			} break;
		}
		else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
			case SDLK_1: chip.keys[1] = 0; break;
			case SDLK_2: chip.keys[2] = 0; break;
			case SDLK_3: chip.keys[3] = 0; break;
			case SDLK_4: chip.keys[12] = 0; break;
			case SDLK_q: chip.keys[4] = 0; break;
			case SDLK_w: chip.keys[5] = 0; break;
			case SDLK_e: chip.keys[6] = 0; break;
			case SDLK_r: chip.keys[13] = 0; break;
			case SDLK_a: chip.keys[7] = 0; break;
			case SDLK_s: chip.keys[8] = 0; break;
			case SDLK_d: chip.keys[9] = 0; break;
			case SDLK_f: chip.keys[14] = 0; break;
			case SDLK_z: chip.keys[10] = 0; break;
			case SDLK_x: chip.keys[0] = 0; break;
			case SDLK_c: chip.keys[11] = 0; break;
			case SDLK_v: chip.keys[15] = 0; break;
			case SDLK_LEFT: step = true; break;
			case SDLK_TAB: _emulationState = EmulationState::FINISH; break;
			case SDLK_UP: chip.printData(); break;
			} break;
		}
		//A game file is dragged into the window.
		else if (event.type == SDL_DROPFILE) {
			fileDir = event.drop.file;
			gameName = std::string(fileDir);
			SDL_free(fileDir);
			newGameLoaded = true;
		}
	}
}

//Emulation of CPU loop.
void Emulate::Emulation::emulationLoop() {

	while (_emulationState != EmulationState::STOP || _emulationState == EmulationState::FINISH) {

		//Reloading game.
		if (_emulationState == EmulationState::FINISH) {
			initializeChip(gameName);
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

		//If soundflag is set then play SFX.
		if (chip.soundFlag) {
			Mix_PlayChannel(-1, beep, 0);
			chip.soundFlag = false;
		}
	}
}

Emulate::Emulation::Emulation()
{
}

Emulate::Emulation::~Emulation()
{
}
