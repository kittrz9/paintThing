#ifndef SAVE_H
#define SAVE_H

#include <SDL3/SDL.h>

#include <stdbool.h>

typedef struct {
	uint32_t* savedCanvasPixels;
	uint32_t savedCanvasW;
	uint32_t savedCanvasH;
	uint32_t savedCanvasPitch;

	bool* savingFlag;
} saveDialogUserdata;

void SDLCALL saveCanvasCallback(void* userdata, const char* const* fileList, int filter);

#endif // SAVE_H
