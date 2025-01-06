#include "ui.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// will probably implement actual buttons and stuff later on
#define MAX_SLIDERS 32

uiSlider sliders[MAX_SLIDERS];

uiSlider* selectedSlider;

void drawSliders(SDL_Renderer* renderer) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		if(sliders[i].size == 0) { continue; }
		SDL_FRect rect = {
			.x = sliders[i].x + 5,
			.y = sliders[i].y + 10,
			.w = 10,
			.h = sliders[i].size,
		};
		SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
		SDL_RenderFillRect(renderer, &rect);

		SDL_FRect rect2 = {
			.x = sliders[i].x,
			.y = sliders[i].y + sliders[i].value*sliders[i].size,
			.w = 20,
			.h = 20,
		};
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect2);
	}
	return;
}

void updateSliders(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	if(mouseButtons & SDL_BUTTON_LMASK){
		if(selectedSlider != NULL) {
			if(mousePosY < selectedSlider->y) {
				selectedSlider->value = 0.0f;
			} else if(mousePosY > selectedSlider->y + selectedSlider->size) {
				selectedSlider->value = 1.0f;
			} else {
				selectedSlider->value = (mousePosY - selectedSlider->y) / selectedSlider->size;
			}
		} else {
			for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
				if(sliders[i].size == 0) { continue; } // maybe should just make it being allocated its own flag for readability
				if(mousePosX > sliders[i].x && mousePosX < sliders[i].x + 20 && mousePosY > sliders[i].y && mousePosY < sliders[i].y + sliders[i].size) {
					selectedSlider = &sliders[i];
					break;
				}
			}
		}
	} else {
		selectedSlider = NULL;
	}
}

uiSlider* createSlider(float x, float y, float size) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		if(sliders[i].size == 0) {
			sliders[i].x = x;
			sliders[i].y = y;
			sliders[i].size = size;
			sliders[i].value = 0;
			return &sliders[i];
		}
	}
	printf("could not allocate slider!\n");
	return NULL;
}

void destroySlider(uiSlider* slider) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		if(slider == &sliders[i]) {
			sliders[i].size = 0;
			return;
		}
	}
	printf("could not destroy slider!\n");
}


SDL_Texture* fontTexture;
// font stored in ascii order starting with !
// probably should just include the font directly in the code to avoid finding and loading a file that could be moved or deleted
void loadFont(SDL_Renderer* renderer, char* fontPath) {
	SDL_Surface* fontSurface = SDL_LoadBMP(fontPath);

	if(fontSurface == NULL) {
		printf("could not load font file \"%s\", %s\n", fontPath, SDL_GetError());
		exit(1);
	}

	fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
	SDL_SetTextureScaleMode(fontTexture, SDL_SCALEMODE_NEAREST);

	SDL_DestroySurface(fontSurface);
}

void drawText(SDL_Renderer* renderer, char* str, float x, float y, float scale) {
	SDL_FRect charRect = {
		.x = 0,
		.y = 0,
		.w = 8,
		.h = 16,
	};
	SDL_FRect screenRect = {
		.x = x,
		.y = y,
		.w = 8*scale,
		.h = 16*scale,
	};
	for(uint32_t i = 0; i < strlen(str); ++i) {
		charRect.x = 8*(str[i]-'!');
		SDL_RenderTexture(renderer, fontTexture, &charRect, &screenRect);
		screenRect.x += 8*scale;
	}
}

// this maybe doesn't matter if all the memory used by SDL gets freed once the program exits
// but idk maybe stuff sent to the gpu doesn't get freed and this causes a tiny memory leak
void unloadFont(void) {
	SDL_DestroyTexture(fontTexture);
}
