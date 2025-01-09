#ifndef SAVE_MODE_H
#define SAVE_MODE_H

#include "modes.h"

void saveModeInit(SDL_Renderer* renderer);
void saveModeUninit(void);
void saveModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

static const mode saveMode = {
	.modeInit = saveModeInit,
	.modeUninit = saveModeUninit,
	.modeRun = saveModeRun,
};

#endif // SAVE_MODE_H
