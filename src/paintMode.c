#include "paintMode.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX(x,y) (x > y ? x : y)
#define MIN(x,y) (x < y ? x : y)

#include "ui.h"
#include "canvas.h"

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

SDL_FRect canvasSrcRect = {
	.x = 0,
	.y = 0,
	.w = CANVAS_WIDTH,
	.h = CANVAS_HEIGHT,
};

uint8_t brushSize = 1;
uint32_t brushColor = 0x0000ffff;

uint32_t brushStartX;

uint32_t palette[] = {
	0x000000ff,
	0x777777ff,
	0xffffffff,
	0xff0000ff,
	0x00ff00ff,
	0x0000ffff,
	0x00ffffff,
	0xff00ffff,
	0xffff00ff,
};

// could just use the color stored in the buttons instead of the palette array since they're both kept in sync
uiButton* paletteButtons[sizeof(palette)/sizeof(palette[0])];

uiSlider* sliderR;
uiSlider* sliderG;
uiSlider* sliderB;

uiSlider* ditherSlider;
uiButton* ditherButton;

uint8_t selectedColor = 0xff;

bool saving = false;

bool dither = false;

void paletteButtonCallback(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	uint8_t index = mousePosY/50;
	if(mouseButtons & SDL_BUTTON_LMASK) {
		brushColor = palette[index];
	} else if(mouseButtons & SDL_BUTTON_RMASK) {
		if(sliderR != NULL) { destroySlider(sliderR); }
		if(sliderG != NULL) { destroySlider(sliderG); }
		if(sliderB != NULL) { destroySlider(sliderB); }
		selectedColor = index;
		uint8_t r = (palette[selectedColor] >> 24) & 0xff;
		uint8_t g = (palette[selectedColor] >> 16) & 0xff;
		uint8_t b = (palette[selectedColor] >> 8) & 0xff;
		sliderR = createSlider(220, selectedColor*50 + 25, 300);
		sliderG = createSlider(260, selectedColor*50 + 25, 300);
		sliderB = createSlider(300, selectedColor*50 + 25, 300);
		sliderR->value = (r/255.0);
		sliderG->value = (g/255.0);
		sliderB->value = (b/255.0);
	}
}

void ditherButtonCallback(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	dither = !dither;
	if(dither) {
		ditherButton->color = 0x2266aaff;
	} else { 
		ditherButton->color = 0x555555ff;
	}
}

void paintModeInit(SDL_Renderer* renderer) {
	for(uint8_t i = 0; i < sizeof(palette)/sizeof(palette[0]); ++i) {
		paletteButtons[i] = createButton(5,i*50 + 5,DISPLAY_X - 10,45,NULL,palette[i],paletteButtonCallback);
	}
	ditherButton = createButton(SCREEN_WIDTH - (DISPLAY_X/2) - 20, 75, 50, 50, NULL, 0x555555ff, ditherButtonCallback);
	ditherSlider = createSlider(SCREEN_WIDTH - (DISPLAY_X/2), 150, 300);
}

void paintModeUninit(void) {
	resetUI();
}

void paintModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	int16_t brushX = ((mousePosX-DISPLAY_X)*(canvasSrcRect.w/(float)DISPLAY_WIDTH)) + canvasSrcRect.x;
	int16_t brushY = ((mousePosY-DISPLAY_Y)*(canvasSrcRect.h/(float)DISPLAY_HEIGHT)) + canvasSrcRect.y;
	switch(e->type) {
		case SDL_EVENT_QUIT:
			running = false;
			break;

		case SDL_EVENT_MOUSE_WHEEL:
			if(SDL_GetModState() & SDL_KMOD_LCTRL) {
				canvasSrcRect.w -= e->wheel.y*8;
				canvasSrcRect.h -= e->wheel.y*6;

				// ideally should keep the brush position the same after zooming
				// just zooming in the middle for now until I figure out how to implement that
				canvasSrcRect.x += e->wheel.y*4;
				canvasSrcRect.y += e->wheel.y*3;

				if(canvasSrcRect.w > CANVAS_WIDTH) {
					canvasSrcRect.w = CANVAS_WIDTH;
				}
				if(canvasSrcRect.h > CANVAS_HEIGHT) {
					canvasSrcRect.h = CANVAS_HEIGHT;
				}
				if(canvasSrcRect.w < 4) {
					canvasSrcRect.w = 4;
				}
				if(canvasSrcRect.h < 3) {
					canvasSrcRect.h = 3;
				}
				if(canvasSrcRect.x + canvasSrcRect.w > CANVAS_WIDTH) {
					canvasSrcRect.x = CANVAS_WIDTH - canvasSrcRect.w;
				}
				if(canvasSrcRect.y + canvasSrcRect.h > CANVAS_HEIGHT) {
					canvasSrcRect.y = CANVAS_HEIGHT - canvasSrcRect.h;
				}
				if(canvasSrcRect.x < 0) {
					canvasSrcRect.x = 0;
				}
				if(canvasSrcRect.y < 0) {
					canvasSrcRect.y = 0;
				}
			} else {
				if((e->wheel.y < 0 && brushSize > 1) || (e->wheel.y > 0 && brushSize < 255)) {
					brushSize += e->wheel.y;
				}
			}
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if(e->button.x > DISPLAY_X && e->button.x < DISPLAY_X + DISPLAY_WIDTH) {
				if(e->button.button == SDL_BUTTON_RIGHT) {
					if(selectedColor != 0xff) {
						brushColor = palette[selectedColor];
						selectedColor = 0xff;
						destroySlider(sliderR);
						destroySlider(sliderG);
						destroySlider(sliderB);
					} else {
						brushColor = canvasGetPixel(brushX, brushY);
					}
				} else if(e->button.button == SDL_BUTTON_LEFT) {
					brushStartX = e->button.x;
				}
			}
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			if(selectedColor == 0xff && e->button.button == SDL_BUTTON_LEFT && brushStartX > DISPLAY_X && brushStartX < DISPLAY_X + DISPLAY_WIDTH) {
				updateCanvasHistory();
			}
			brushStartX = 0;
			break;

		case SDL_EVENT_MOUSE_MOTION:
			if(mouseButtons & SDL_BUTTON_MMASK) {
				canvasSrcRect.x -= e->motion.xrel/2;
				canvasSrcRect.y -= e->motion.yrel/2;

				if(canvasSrcRect.x + canvasSrcRect.w > CANVAS_WIDTH) {
					canvasSrcRect.x = CANVAS_WIDTH - canvasSrcRect.w;
				}
				if(canvasSrcRect.y + canvasSrcRect.h > CANVAS_HEIGHT) {
					canvasSrcRect.y = CANVAS_HEIGHT - canvasSrcRect.h;
				}
				if(canvasSrcRect.x < 0) {
					canvasSrcRect.x = 0;
				}
				if(canvasSrcRect.y < 0) {
					canvasSrcRect.y = 0;
				}
			}
			break;

		case SDL_EVENT_KEY_DOWN:
			if(e->key.mod & SDL_KMOD_LCTRL) {
				switch(e->key.key) {
					case SDLK_Z:
						undoCanvas();
						break;

					case SDLK_S:
						modeSwitch(&saveMode, renderer);
						return;
					default: break;
				}
			}
			break;

		default: break;
	}

	updateUI(e, mousePosX, mousePosY, mouseButtons);

	SDL_SetRenderDrawColor(renderer, 125, 112, 104, 255);
	SDL_RenderClear(renderer);

	lockCanvasTexture();
	if(!saving && selectedColor == 0xff) {
		for(uint16_t y = MAX(0, brushY-brushSize); y < MIN(CANVAS_HEIGHT, brushY+brushSize); ++y) {
			for(uint16_t x = MAX(0, brushX-brushSize); x < MIN(CANVAS_WIDTH, brushX+brushSize); ++x) {
				float ditherSize = (ditherSlider->value * 20) + 1;
				if(dither && (x/(uint32_t)ditherSize+y/(uint32_t)ditherSize) % 2 == 0) {
					continue;
				}
				// jank to keep both pointers in sync
				uint32_t x2 = x - brushX;
				uint32_t y2 = y - brushY;
				if(x2*x2 + y2*y2 < brushSize*brushSize) {
					// draw brush preview
					canvasTextureSetPixel(x, y, brushColor);
					if(mouseButtons & SDL_BUTTON_LMASK && brushStartX > DISPLAY_X && brushStartX < DISPLAY_X + DISPLAY_WIDTH) {
						// draw brush to the actual canvas
						canvasSetPixel(x, y, brushColor);
					}
				}
			}
		}
	}
	unlockCanvasTexture();

	SDL_FRect colorDisplayRect = {
		.x = 0,
		.y = 0,
		.w = 50,
		.h = 50,
	};

	drawCanvas(renderer, &displayRect, &canvasSrcRect); 

	if(selectedColor != 0xff) {
		SDL_SetRenderDrawColor(renderer,0,0,0,0xc0);
		SDL_RenderFillRect(renderer, &(SDL_FRect){200,50*selectedColor,300,400});

		SDL_SetRenderDrawColor(renderer,255,255,255,255);

		uint8_t r = sliderR->value*255;
		uint8_t g = sliderG->value*255;
		uint8_t b = sliderB->value*255;
		palette[selectedColor] = (r<<24) | (g<<16) | (b<<8) | 0xff;
		paletteButtons[selectedColor]->color = (r<<24) | (g<<16) | (b<<8) | 0xff;
	}

	drawUI(renderer);

	if(saving) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xc0);
		SDL_RenderFillRect(renderer, NULL);
	}

	drawText(renderer, "dither:", SCREEN_WIDTH-150, 25, 2.0);

	drawText(renderer, "test string", 0,SCREEN_HEIGHT-32,2.0);
}
