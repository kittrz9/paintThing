#include "modes.h"

const mode* currentMode;

bool running = true;

void modeSwitch(const mode* newMode, SDL_Renderer* renderer) {
	if(currentMode != NULL) {
		(*currentMode->modeUninit)();
	}

	if(newMode != NULL) {
		(*newMode->modeInit)(renderer);
	}

	currentMode = newMode;
}

void modeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	(*currentMode->modeRun)(renderer, e, mousePosX, mousePosY, mouseButtons);
}
