#ifndef PAINT_MODE_H
#define PAINT_MODE_H

#include "modes.h"

void paintModeInit(SDL_Renderer* renderer);
void paintModeUninit(void);
void paintModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

static const mode paintMode = {
	.modeInit = paintModeInit,
	.modeUninit = paintModeUninit,
	.modeRun = paintModeRun,
};

#endif // PAINT_MODE_H
