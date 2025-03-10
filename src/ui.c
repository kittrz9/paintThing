#include "ui.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "generated/font.h"

#include <lzma.h>

SDL_Window* window;

#define MAX_SLIDERS 32

uiSlider sliders[MAX_SLIDERS];
uiSlider* selectedSlider;

#define MAX_BUTTONS 32
uiButton buttons[MAX_BUTTONS];

#define MAX_CHECKBOXES 32
uiCheckbox checkboxes[MAX_CHECKBOXES];

void drawUI(SDL_Renderer* renderer) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		if(!sliders[i].allocated) { continue; }
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

	for(uint32_t i = 0; i < MAX_BUTTONS; ++i) {
		if(!buttons[i].allocated) { continue; }
		SDL_FRect rect = {
			.x = buttons[i].x,
			.y = buttons[i].y,
			.w = buttons[i].w,
			.h = buttons[i].h,
		};
		// need to draw the buttons as textures eventually
		if(buttons[i].texture == NULL) {
			uint8_t r = (buttons[i].color >> 24) & 0xff;
			uint8_t g = (buttons[i].color >> 16) & 0xff;
			uint8_t b = (buttons[i].color >> 8) & 0xff;
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	for(uint32_t i = 0; i < MAX_CHECKBOXES; ++i) {
		if(!checkboxes[i].allocated) { continue; }
		SDL_FRect rect = {
			.x = checkboxes[i].x,
			.y = checkboxes[i].y,
			.w = checkboxes[i].size,
			.h = checkboxes[i].size,
		};
		if(*checkboxes[i].flag) {
			SDL_SetRenderDrawColor(renderer, 0x22, 0x66, 0xaa, 0xff);
		} else {
			SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xff);
		}
		SDL_RenderFillRect(renderer, &rect);
	}
	return;
}

void updateUI(SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
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
				if(!sliders[i].allocated) { continue; }
				if(mousePosX > sliders[i].x && mousePosX < sliders[i].x + 20 && mousePosY > sliders[i].y && mousePosY < sliders[i].y + sliders[i].size) {
					selectedSlider = &sliders[i];
					break;
				}
			}
		}
	} else {
		selectedSlider = NULL;
	}

	switch(e->type) {
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			for(uint32_t i = 0; i < MAX_BUTTONS; ++i) {
				if(buttons[i].allocated && mousePosX > buttons[i].x && mousePosX < buttons[i].x + buttons[i].w && mousePosY > buttons[i].y && mousePosY < buttons[i].y + buttons[i].h) {
					buttons[i].clickCallback(mousePosX, mousePosY, mouseButtons);
				}
			}
			for(uint32_t i = 0; i < MAX_CHECKBOXES; ++i) {
				if(checkboxes[i].allocated && mousePosX > checkboxes[i].x && mousePosX < checkboxes[i].x + checkboxes[i].size && mousePosY > checkboxes[i].y && mousePosY < checkboxes[i].y + checkboxes[i].size) {
					*checkboxes[i].flag = !*checkboxes[i].flag;
				}
			}
			break;
	}
}

void resetUI(void) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		sliders[i].allocated = false;
	}
	for(uint32_t i = 0; i < MAX_BUTTONS; ++i) {
		buttons[i].allocated = false;
	}
	for(uint32_t i = 0; i < MAX_CHECKBOXES; ++i) {
		checkboxes[i].allocated = false;
	}
}

uiSlider* createSlider(float x, float y, float size) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		if(!sliders[i].allocated) {
			sliders[i].x = x;
			sliders[i].y = y;
			sliders[i].size = size;
			sliders[i].value = 0;
			sliders[i].allocated = true;
			return &sliders[i];
		}
	}
	printf("could not allocate slider!\n");
	return NULL;
}

void destroySlider(uiSlider* slider) {
	for(uint32_t i = 0; i < MAX_SLIDERS; ++i) {
		if(slider == &sliders[i]) {
			sliders[i].allocated = false;
			return;
		}
	}
	printf("could not destroy slider!\n");
}

uiButton* createButton(float x, float y, float w, float h, SDL_Texture* texture, uint32_t color, void (*clickCallback)(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons)) {
	for(uint32_t i = 0; i < MAX_BUTTONS; ++i) {
		if(!buttons[i].allocated) {
			buttons[i].x = x;
			buttons[i].y = y;
			buttons[i].w = w;
			buttons[i].h = h;
			buttons[i].texture = texture;
			buttons[i].color = color;
			buttons[i].clickCallback = clickCallback;
			buttons[i].allocated = true;
			return &buttons[i];
		}
	}
	printf("could not allocate button!\n");
	return NULL;
}
void destroyButton(uiButton* button) {
}

uiCheckbox* createCheckbox(float x, float y, float size, bool* flag) {
	for(uint32_t i = 0; i < MAX_CHECKBOXES; ++i) {
		if(!checkboxes[i].allocated) {
			checkboxes[i].x = x;
			checkboxes[i].y = y;
			checkboxes[i].size = size;
			checkboxes[i].flag = flag;
			checkboxes[i].allocated = true;
			return &checkboxes[i];
		}
	}
	printf("could not allocate check box!\n");
	return NULL;
}
void destroyCheckbox(uiCheckbox* checkbox) {
}


SDL_Texture* fontTexture;
// font stored in ascii order starting with !
void loadFont(SDL_Renderer* renderer) {
	uint8_t* fontImg = malloc(4*fontImgW*fontImgH);
	uint64_t memLimit = 67108864; // 64M
	size_t inPos = 0;
	size_t outPos = 0;
	lzma_stream_buffer_decode(&memLimit, 0, NULL, fontImgLZMA, &inPos, fontImgLZMALen, fontImg, &outPos, 4*fontImgW*fontImgH);
	SDL_Surface* fontSurface = SDL_CreateSurfaceFrom(fontImgW, fontImgH, SDL_PIXELFORMAT_RGBA32, fontImg, fontImgW * 4);

	if(fontSurface == NULL) {
		printf("could not load font, %s\n", SDL_GetError());
		exit(1);
	}

	fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
	SDL_SetTextureScaleMode(fontTexture, SDL_SCALEMODE_NEAREST);

	SDL_DestroySurface(fontSurface);
	free(fontImg);
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
