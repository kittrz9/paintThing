#include <stdio.h>
#include <SDL3/SDL.h>

#include "ui.h"
#include "modes.h"
#include "canvas.h"

// should get the framerate of the current monitor, just hard coded to my monitor's refresh rate for now
#define FRAMERATE 144

SDL_Window* window;

SDL_Renderer* renderer;

int main(int argc, char** argv) {
	if(SDL_Init(SDL_INIT_VIDEO) == 0) {
		printf("could not initialize sdl\n");
		return 1;
	}

	window = SDL_CreateWindow("paintTest", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, NULL);

	if(renderer == NULL) {
		printf("could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	initCanvas(renderer);

	loadFont(renderer, "font.bmp");

	modeSwitch(&paintMode, renderer);


	uint64_t start;
	uint64_t end;

	SDL_Event e;
	while(running) {
		start = SDL_GetTicksNS();
		SDL_PollEvent(&e);

		float mousePosX;
		float mousePosY;
		SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mousePosX, &mousePosY);
		modeRun(renderer, &e, mousePosX, mousePosY, mouseButtons);

		SDL_RenderPresent(renderer);

		end = SDL_GetTicksNS();

		if(end-start < 1000000000/FRAMERATE) {
			SDL_DelayNS(1000000000/FRAMERATE - (end-start));
		}
		// with how SDL3 is now it lags when too many mouse movement events happen with the framerate limited
		// getting rid of all of the events between frames seems to fix this
		SDL_PumpEvents();

	}

	modeSwitch(NULL, NULL);

	unloadFont();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
