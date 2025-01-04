#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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
uint32_t* texturePixels;
uint32_t* canvasPixels;
int canvasPitch;

uint8_t brushSize = 1;
uint32_t brushColor = 0x0000ffff;

uint32_t palette[] = {
	0x000000ff,
	0xffffffff,
	0xff0000ff,
	0x00ff00ff,
	0x0000ffff,
};

float sliderR = 0.0f;
float sliderG = 0.0f;
float sliderB = 0.0f;

uint8_t selectedColor = 0xff;
float* selectedSlider;

bool running = true;

void copyCanvas(uint32_t* canvasDst, uint32_t* canvasSrc) {
	for(uint16_t y = 0; y < CANVAS_HEIGHT; ++y) {
		memcpy(canvasDst, canvasSrc, canvasPitch);
		canvasDst = (uint32_t*)((uint8_t*)canvasDst + canvasPitch);
		canvasSrc = (uint32_t*)((uint8_t*)canvasSrc + canvasPitch);
	}
}

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

	canvasTexture = SDL_CreateTexture(renderer, canvasFormat, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);

	SDL_LockTexture(canvasTexture, NULL, &texturePixels, &canvasPitch);
	canvasPixels = malloc(sizeof(uint32_t) * canvasPitch * CANVAS_HEIGHT);
	memset(canvasPixels, 255, sizeof(uint32_t) * canvasPitch * CANVAS_HEIGHT);
	SDL_UnlockTexture(canvasTexture);

	SDL_SetTextureScaleMode(canvasTexture, SDL_SCALEMODE_NEAREST);


	SDL_Event e;
	while(running) {
		SDL_PollEvent(&e);
		switch(e.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;

			case SDL_EVENT_MOUSE_WHEEL:
				if((e.wheel.y < 0 && brushSize > 1) || (e.wheel.y > 0 && brushSize < 255)) {
					brushSize += e.wheel.y;
				}
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if(e.button.x < DISPLAY_X) {
					for(uint8_t i = 0; i < sizeof(palette)/sizeof(palette[0]); ++i) {
						if(e.button.y > i*50 && e.button.y < (i+1) * 50) {
							if(e.button.button == SDL_BUTTON_LEFT) {
								brushColor = palette[i];
								break;
							}
							if(e.button.button == SDL_BUTTON_RIGHT) {
								selectedColor = i;
								break;
							}
						}
					}
				} else {
					if(e.button.button == SDL_BUTTON_RIGHT) {
						selectedColor = 0xff;
						break;
					}
				}
				break;

			default: break;
		}

		float mousePosX;
		float mousePosY;
		SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mousePosX, &mousePosY);

		SDL_SetRenderDrawColor(renderer, 125, 112, 104, 255);
		SDL_RenderClear(renderer);

		SDL_LockTexture(canvasTexture, NULL, &texturePixels, &canvasPitch);
		copyCanvas(texturePixels, canvasPixels);
		if(selectedColor == 0xff) {
			uint32_t* canvasPixel = canvasPixels;
			uint32_t* texturePixel = texturePixels;
			for(uint16_t y = 0; y < CANVAS_HEIGHT; ++y) {
				for(uint16_t x = 0; x < CANVAS_WIDTH; ++x) {
					// jank to keep both pointers in sync
					uint32_t offset = (uint32_t)canvasPixel - (uint32_t)canvasPixels;
					texturePixel = (uint32_t*)((uint8_t*)texturePixels + offset);

					uint32_t x2 = x - ((mousePosX-DISPLAY_X)*((float)CANVAS_WIDTH/(float)DISPLAY_WIDTH));
					uint32_t y2 = y - ((mousePosY-DISPLAY_Y)*((float)CANVAS_HEIGHT/(float)DISPLAY_HEIGHT));
					if(x2*x2 + y2*y2 < brushSize*brushSize) {
						// draw brush preview
						*texturePixel = brushColor;
						if(mouseButtons & SDL_BUTTON_LMASK) {
							// draw brush to the actual canvas
							*canvasPixel = brushColor;
						}
						if(mouseButtons & SDL_BUTTON_RMASK) {
							*canvasPixel = 0xffffffff;
						}
					}
					++canvasPixel;
				}
				canvasPixel = (uint32_t*)((uint8_t*)canvasPixels + (canvasPitch * y)); // casts to uint8_t* and back to move forward in single bytes rather than 4 bytes at a time, casting it back avoids a compiler warning
			}
		}
		SDL_UnlockTexture(canvasTexture);


		SDL_FRect colorDisplayRect = {
			.x = 0,
			.y = 0,
			.w = 50,
			.h = 50,
		};
		for(uint8_t i = 0; i < sizeof(palette)/sizeof(palette[0]); ++i) {
			uint8_t r = (palette[i] >> 24) & 0xff;
			uint8_t g = (palette[i] >> 16) & 0xff;
			uint8_t b = (palette[i] >> 8) & 0xff;
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderFillRect(renderer, &colorDisplayRect);
			colorDisplayRect.y += 50;
		}

		SDL_RenderTexture(renderer, canvasTexture, NULL, &displayRect);

		if(selectedColor != 0xff) {
			// really need to set up an actual ui manager thing
			SDL_SetRenderDrawColor(renderer,0,0,0,0xc0);
			SDL_RenderFillRect(renderer, &(SDL_FRect){200,50*selectedColor,300,400});

			SDL_SetRenderDrawColor(renderer,255,255,255,255);
			uint8_t r = (palette[selectedColor] >> 24) & 0xff;
			uint8_t g = (palette[selectedColor] >> 16) & 0xff;
			uint8_t b = (palette[selectedColor] >> 8) & 0xff;
			sliderR = (r/255.0);
			sliderG = (g/255.0);
			sliderB = (b/255.0);
			SDL_RenderFillRect(renderer, &(SDL_FRect){220,50*selectedColor + (r/255.0)*200, 20, 20});
			SDL_RenderFillRect(renderer, &(SDL_FRect){260,50*selectedColor + (g/255.0)*200, 20, 20});
			SDL_RenderFillRect(renderer, &(SDL_FRect){300,50*selectedColor + (b/255.0)*200, 20, 20});

			if(mouseButtons & SDL_BUTTON_LEFT) {
				if(mousePosX > 220 && mousePosX < 260) {
					selectedSlider = &sliderR;
				}
				if(mousePosX > 260 && mousePosX < 300) {
					selectedSlider = &sliderG;
				}
				if(mousePosX > 300 && mousePosX < 340) {
					selectedSlider = &sliderB;
				}
				if(selectedSlider != NULL) {
					*selectedSlider = (mousePosY - 50*selectedColor) / 400.0;
				}
			} else {
				selectedSlider = NULL;
			}

			r = sliderR*255;
			g = sliderG*255;
			b = sliderB*255;
			palette[selectedColor] = (r<<24) | (g<<16) | (b<<8) | 0xff;
		}

		SDL_RenderPresent(renderer);
	}

	return 0;
}
