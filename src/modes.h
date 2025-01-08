#ifndef MODES_H
#define MODES_H

#include <SDL3/SDL.h>

#include <stdbool.h>

typedef struct {
	void (*modeInit)(SDL_Renderer* renderer);
	void (*modeUninit)(void);
	void (*modeRun)(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);
} mode;

void modeSwitch(const mode* newMode, SDL_Renderer* renderer);
void modeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

extern bool running;

#include "paintMode.h"

#endif // MODES_H
