#include <stdio.h>
#include <SDL3/SDL.h>

#include "ui.h"
#include "modes.h"

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


	loadFont(renderer, "font.bmp");

	modeSwitch(&canvasMode, renderer);

	SDL_Event e;
	while(running) {
		SDL_PollEvent(&e);

		float mousePosX;
		float mousePosY;
		SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mousePosX, &mousePosY);
		modeRun(renderer, &e, mousePosX, mousePosY, mouseButtons);

		SDL_RenderPresent(renderer);
	}

	modeSwitch(NULL, NULL);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
