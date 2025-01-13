#include "canvas.h"

#include <SDL3/SDL.h>

#include <stdlib.h>
#include <stdio.h>

canvasData canvas;

SDL_Texture* canvasTexture;

SDL_PixelFormat canvasFormat = SDL_PIXELFORMAT_ABGR32; // since this is on x86 this lets me set colors using hexadecimal in RGBA format, has to be converted to big endian when saving though
uint32_t* texturePixels;
uint32_t* canvasPixels;
int canvasPitch;

#define MAX_UNDO 32
uint32_t* canvasHistory[MAX_UNDO];
uint8_t historyIndex = 0;

void copyCanvas(uint32_t* canvasDst, uint32_t* canvasSrc) {
	for(uint16_t y = 0; y < CANVAS_HEIGHT; ++y) {
		memcpy(canvasDst, canvasSrc, canvasPitch);
		canvasDst = (uint32_t*)((uint8_t*)canvasDst + canvasPitch);
		canvasSrc = (uint32_t*)((uint8_t*)canvasSrc + canvasPitch);
	}
}

void initCanvas(SDL_Renderer* renderer) {
	canvasTexture = SDL_CreateTexture(renderer, canvasFormat, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);

	SDL_LockTexture(canvasTexture, NULL, (void**)&texturePixels, &canvasPitch);
	canvasPixels = malloc(canvasPitch * CANVAS_HEIGHT);
	memset(canvasPixels, 255, canvasPitch * CANVAS_HEIGHT);

	for(uint8_t i = 0; i < MAX_UNDO; ++i) {
		canvasHistory[i] = malloc(canvasPitch * CANVAS_HEIGHT);
		memset(canvasHistory[i], 255,canvasPitch * CANVAS_HEIGHT);
	}
	SDL_UnlockTexture(canvasTexture);

	SDL_SetTextureScaleMode(canvasTexture, SDL_SCALEMODE_NEAREST);

	canvas.w = CANVAS_WIDTH;
	canvas.h = CANVAS_HEIGHT;
	canvas.pixels = canvasPixels;
	canvas.pitch = canvasPitch;
}

void uninitCanvas(void) {
	free(canvasPixels);
	for(uint8_t i = 0; i < MAX_UNDO; ++i) {
		free(canvasHistory[i]);
	}
}

void lockCanvasTexture(void) {
	SDL_LockTexture(canvasTexture, NULL, (void**)&texturePixels, &canvasPitch);
	copyCanvas(texturePixels, canvasPixels);
}

void unlockCanvasTexture(void) {
	SDL_UnlockTexture(canvasTexture);
}

void canvasTextureSetPixel(uint32_t x, uint32_t y, uint32_t color) {
	uint32_t offset = (y*canvasPitch)+(x*sizeof(uint32_t));
	uint32_t* texturePixel = (uint32_t*)((uint8_t*)texturePixels + offset);

	*texturePixel = color;
}

// maybe should change to a more distinct name
void canvasSetPixel(uint32_t x, uint32_t y, uint32_t color) {
	uint32_t offset = (y*canvasPitch)+(x*sizeof(uint32_t));
	uint32_t* canvasPixel = (uint32_t*)((uint8_t*)canvasPixels + offset);

	*canvasPixel = color;
}

uint32_t canvasGetPixel(uint32_t x, uint32_t y) {
	uint32_t offset = (y*canvasPitch)+(x*sizeof(uint32_t));
	return  *(uint32_t*)((uint8_t*)canvasPixels + offset);
}

// maybe I shouldn't swap the src and dest rects order here
void drawCanvas(SDL_Renderer* renderer, SDL_FRect* dstRect, SDL_FRect* srcRect) {
	SDL_RenderTexture(renderer, canvasTexture, srcRect, dstRect);
}

void updateCanvasHistory(void) {
	if(historyIndex < MAX_UNDO-1) {
		++historyIndex;
	} else {
		printf("shifting undo history!\n");
		for(uint8_t i = 0; i < historyIndex; ++i) {
			printf("! %i -> %i\n", i+1, i);
			copyCanvas(canvasHistory[i], canvasHistory[i+1]);
		}
	}
	printf("! (canvas) -> %i\n", historyIndex);
	copyCanvas(canvasHistory[historyIndex], canvasPixels);
}

void undoCanvas(void) {
	if(historyIndex > 0) {
		--historyIndex;
	}
	printf("<- %i\n", historyIndex);
	copyCanvas(canvasPixels, canvasHistory[historyIndex]);
}

typedef struct {
	uint32_t x;
	uint32_t y;
} vec2;

#define VEC2_INVALID (vec2){0xffffffff, 0xffffffff}

#define MAX_FFSTACK_SIZE CANVAS_WIDTH*CANVAS_HEIGHT*4
vec2 ffStack[MAX_FFSTACK_SIZE];
uint32_t ffStackPointer;

vec2 ffStackPop(void) {
	if(ffStackPointer == 0) {
		return VEC2_INVALID;
	}
	--ffStackPointer;
	return ffStack[ffStackPointer];
}
void ffStackPush(vec2 v) {
	if(ffStackPointer == MAX_FFSTACK_SIZE) {
		printf("flood fill stack overflow\n");
		exit(1);
	}
	if(v.x > CANVAS_WIDTH || v.y > CANVAS_HEIGHT) { return; }
	ffStack[ffStackPointer] = v;
	++ffStackPointer;
	return;
}

void floodFill(uint32_t x, uint32_t y, uint32_t brushColor) {
	uint32_t startColor = canvasGetPixel(x, y);
	if(startColor == brushColor) { return; }
	ffStackPointer = 0;
	ffStackPush((vec2){x,y});
	while(ffStackPointer != 0) {
		vec2 n = ffStackPop();
		if(canvasGetPixel(n.x, n.y) == startColor) {
			canvasSetPixel(n.x, n.y, brushColor);
			ffStackPush((vec2){n.x-1, n.y});
			ffStackPush((vec2){n.x+1, n.y});
			ffStackPush((vec2){n.x, n.y-1});
			ffStackPush((vec2){n.x, n.y+1});
		}
	}
}

canvasData* getCanvas(void) {
	return &canvas;
}
