#include "ui.h"

#include <stdint.h>
#include <stdio.h>

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
