#include "paintMode.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h> // MAX and MIN macros

#include "ui.h"
#include "save.h"
#include "canvas.h"

#define DISPLAY_WIDTH (SCREEN_WIDTH * (3.0/4.0))
#define DISPLAY_HEIGHT (SCREEN_HEIGHT + 4.0)
#define DISPLAY_X ((SCREEN_WIDTH - DISPLAY_WIDTH)/2.0)
#define DISPLAY_Y 0.0

saveDialogUserdata userdata;

SDL_FRect displayRect = {
	.x = DISPLAY_X,
	.y = DISPLAY_Y,
	.w = DISPLAY_WIDTH,
	.h = DISPLAY_HEIGHT,
};

uint8_t brushSize = 1;
uint32_t brushColor = 0x0000ffff;

uint32_t palette[] = {
	0x000000ff,
	0x777777ff,
	0xffffffff,
	0xff0000ff,
	0x00ff00ff,
	0x0000ffff,
};

uiSlider* sliderR;
uiSlider* sliderG;
uiSlider* sliderB;

uint8_t selectedColor = 0xff;

bool saving = false;

void paintModeInit(SDL_Renderer* renderer) {
	// currently empty, would probably create any ui elements here
}

void paintModeUninit(void) {
	// this is also empty for now until I make it create ui elements
}

void paintModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	switch(e->type) {
		case SDL_EVENT_QUIT:
			running = false;
			break;

		case SDL_EVENT_MOUSE_WHEEL:
			if((e->wheel.y < 0 && brushSize > 1) || (e->wheel.y > 0 && brushSize < 255)) {
				brushSize += e->wheel.y;
			}
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if(e->button.x < DISPLAY_X) {
				for(uint8_t i = 0; i < sizeof(palette)/sizeof(palette[0]); ++i) {
					if(e->button.y > i*50 && e->button.y < (i+1) * 50) {
						if(e->button.button == SDL_BUTTON_LEFT) {
							brushColor = palette[i];
							break;
						}
						if(e->button.button == SDL_BUTTON_RIGHT) {
							if(sliderR != NULL) { destroySlider(sliderR); }
							if(sliderG != NULL) { destroySlider(sliderG); }
							if(sliderB != NULL) { destroySlider(sliderB); }
							selectedColor = i;
							uint8_t r = (palette[selectedColor] >> 24) & 0xff;
							uint8_t g = (palette[selectedColor] >> 16) & 0xff;
							uint8_t b = (palette[selectedColor] >> 8) & 0xff;
							sliderR = createSlider(220, selectedColor*50 + 25, 300);
							sliderG = createSlider(260, selectedColor*50 + 25, 300);
							sliderB = createSlider(300, selectedColor*50 + 25, 300);
							sliderR->value = (r/255.0);
							sliderG->value = (g/255.0);
							sliderB->value = (b/255.0);
							break;
						}
					}
				}
			} else {
				if(e->button.button == SDL_BUTTON_RIGHT) {
					selectedColor = 0xff;
					destroySlider(sliderR);
					destroySlider(sliderG);
					destroySlider(sliderB);
					break;
				}
			}
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			// if the left mouse button stops being held in the color picker area it wont add whatever brush stroke that was into the history, not really a big deal since most brush strokes will end up stopping in the canvas but maybe I should fix this anyways
			if(selectedColor == 0xff && e->button.button == SDL_BUTTON_LEFT && e->button.x > DISPLAY_X) {
				updateCanvasHistory();
			}
			break;

		case SDL_EVENT_KEY_DOWN:
			if(e->key.mod & SDL_KMOD_LCTRL) {
				switch(e->key.key) {
					case SDLK_Z:
						undoCanvas();
						break;

					case SDLK_S:
						// would prefer to have the file saving stuff part of the actual ui eventually
						// would need to implement text drawing and input stuff though
						userdata = (saveDialogUserdata){
							.canvas = getCanvas(),
							.savingFlag = &saving,
						};
						saving = true;
						SDL_ShowSaveFileDialog(saveCanvasCallback, (void*)&userdata, NULL, saveDialogFilters, sizeof(saveDialogFilters)/sizeof(saveDialogFilters[0]), NULL);
						break;
					default: break;
				}
			}
			break;

		default: break;
	}

	updateSliders(mousePosX, mousePosY, mouseButtons);

	SDL_SetRenderDrawColor(renderer, 125, 112, 104, 255);
	SDL_RenderClear(renderer);

	lockCanvasTexture();
	if(!saving && selectedColor == 0xff) {
		int16_t brushX = ((mousePosX-DISPLAY_X)*((float)CANVAS_WIDTH/(float)DISPLAY_WIDTH));
		int16_t brushY = ((mousePosY-DISPLAY_Y)*((float)CANVAS_HEIGHT/(float)DISPLAY_HEIGHT));
		for(uint16_t y = MAX(0, brushY-brushSize); y < MIN(CANVAS_HEIGHT, brushY+brushSize); ++y) {
			for(uint16_t x = MAX(0, brushX-brushSize); x < MIN(CANVAS_WIDTH, brushX+brushSize); ++x) {
				// jank to keep both pointers in sync
				uint32_t x2 = x - brushX;
				uint32_t y2 = y - brushY;
				if(x2*x2 + y2*y2 < brushSize*brushSize) {
					// draw brush preview
					canvasTextureSetPixel(x, y, brushColor);
					if(mouseButtons & SDL_BUTTON_LMASK) {
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
	for(uint8_t i = 0; i < sizeof(palette)/sizeof(palette[0]); ++i) {
		uint8_t r = (palette[i] >> 24) & 0xff;
		uint8_t g = (palette[i] >> 16) & 0xff;
		uint8_t b = (palette[i] >> 8) & 0xff;
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		SDL_RenderFillRect(renderer, &colorDisplayRect);
		colorDisplayRect.y += 50;
	}

	drawCanvas(renderer, DISPLAY_X, DISPLAY_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT); 

	if(selectedColor != 0xff) {
		SDL_SetRenderDrawColor(renderer,0,0,0,0xc0);
		SDL_RenderFillRect(renderer, &(SDL_FRect){200,50*selectedColor,300,400});

		SDL_SetRenderDrawColor(renderer,255,255,255,255);

		uint8_t r = sliderR->value*255;
		uint8_t g = sliderG->value*255;
		uint8_t b = sliderB->value*255;
		palette[selectedColor] = (r<<24) | (g<<16) | (b<<8) | 0xff;
	}

	drawSliders(renderer);

	if(saving) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xc0);
		SDL_RenderFillRect(renderer, NULL);
	}

	drawText(renderer, "test string", 0,SCREEN_HEIGHT-32,2.0);
}
