#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
SDL_Window* window;

SDL_Renderer* renderer;

#define CANVAS_WIDTH 640
#define CANVAS_HEIGHT 480

// the aspect ratio of the pixels gets messed up for some reason, probably due to SDL's weird new subpixel precision rendering stuff
// could maybe just blit the texture to the screen directly but I want to eventually start using the actual renderer features
#define DISPLAY_WIDTH (SCREEN_WIDTH * (3.0/4.0))
#define DISPLAY_HEIGHT (SCREEN_HEIGHT + 4.0)
#define DISPLAY_X ((SCREEN_WIDTH - DISPLAY_WIDTH)/2.0)
#define DISPLAY_Y 0.0

SDL_FRect displayRect = {
	.x = DISPLAY_X,
	.y = DISPLAY_Y,
	.w = DISPLAY_WIDTH,
	.h = DISPLAY_HEIGHT,
};

SDL_Texture* canvasTexture;

SDL_PixelFormat canvasFormat = SDL_PIXELFORMAT_ABGR32; // SDL probably tries to make little/big endian not matter here, but trying RGBA32 ends up with it reversed and this fixes that
uint32_t* canvasPixels;
int canvasPitch;

bool running = true;

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

	canvasTexture = SDL_CreateTexture(renderer, canvasFormat, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);

	SDL_SetTextureScaleMode(canvasTexture, SDL_SCALEMODE_NEAREST);

	SDL_LockTexture(canvasTexture, NULL, &canvasPixels, &canvasPitch);
	uint32_t* pixel = canvasPixels;
	for(uint16_t y = 0; y < CANVAS_HEIGHT; ++y) {
		for(uint16_t x = 0; x < CANVAS_WIDTH; ++x) {
			/*if((x/4 + y/4) % 2 == 0) {
				*pixel = 0x000000ff;
			} else {
				*pixel = 0xffffffff;
			}*/
			*pixel = 0xffffffff;

			uint32_t x2 = x - CANVAS_WIDTH/2;
			uint32_t y2 = y - CANVAS_HEIGHT/2;
			if(x2*x2 + y2*y2 < 100) {
				*pixel = 0x0000ffff;
			}
			++pixel;
		}
		pixel = (uint32_t*)((uint8_t*)canvasPixels + (canvasPitch * y)); // casts to uint8_t* and back to move forward in single bytes rather than 4 bytes at a time, casting it back avoids a compiler warning
	}
	SDL_UnlockTexture(canvasTexture);

	SDL_Event e;
	while(running) {
		SDL_PollEvent(&e);
		switch(e.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;

			default: break;
		}

		SDL_SetRenderDrawColor(renderer, 125, 112, 104, 255);
		SDL_RenderClear(renderer);

		SDL_RenderTexture(renderer, canvasTexture, NULL, &displayRect);

		SDL_RenderPresent(renderer);
	}

	return 0;
}
