#ifndef CANVAS_H
#define CANVAS_H

#include <SDL3/SDL.h>

#define CANVAS_WIDTH 640
#define CANVAS_HEIGHT 480

typedef struct {
	uint32_t* pixels;
	uint32_t w;
	uint32_t h;
	uint32_t pitch;
} canvasData;

void initCanvas(SDL_Renderer* renderer);
void uninitCanvas(void);
void drawCanvas(SDL_Renderer* renderer, float x, float y, float w, float h);

void lockCanvasTexture(void);
void unlockCanvasTexture(void);

void canvasTextureSetPixel(uint32_t x, uint32_t y, uint32_t color);
void canvasSetPixel(uint32_t x, uint32_t y, uint32_t color);

void updateCanvasHistory(void);
void undoCanvas(void);

canvasData* getCanvas(void);

#endif // CANVAS_H
