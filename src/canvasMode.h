#ifndef CANVAS_MODE_H
#define CANVAS_MODE_H

#include "modes.h"

void canvasModeInit(SDL_Renderer* renderer);
void canvasModeUninit(void);
void canvasModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

static const mode canvasMode = {
	.modeInit = canvasModeInit,
	.modeUninit = canvasModeUninit,
	.modeRun = canvasModeRun,
};

#endif // CANVAS_MODE_H
