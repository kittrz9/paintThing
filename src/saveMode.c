#include "saveMode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "ui.h"
#include "canvas.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

SDL_FRect canvasDisplayRect = {
	SCREEN_WIDTH/2 - 200,
	20,
	400,
	300
};

#define MAX_DIR_SIZE 256
#define MAX_DIR_ENTRY_LEN 128

char dirEntries[MAX_DIR_SIZE][MAX_DIR_ENTRY_LEN];
uint32_t currentDirSize;

uint32_t selectedEntry = 0xffffffff;

char currentDir[MAX_DIR_ENTRY_LEN];

float scrollPosition;

char fileName[MAX_DIR_ENTRY_LEN] = "asdfasdf";

bool enteringName;

#define DIR_LIST_X 20
#define DIR_LIST_Y 350
#define DIR_LIST_WIDTH (SCREEN_WIDTH - 40)
#define DIR_LIST_HEIGHT 450

SDL_Rect dirListRect = {
	.x = DIR_LIST_X,
	.y = DIR_LIST_Y,
	.w = DIR_LIST_WIDTH,
	.h = DIR_LIST_HEIGHT,
};

#define NAME_ENTRY_X DIR_LIST_X
#define NAME_ENTRY_Y DIR_LIST_Y + DIR_LIST_HEIGHT + 22
#define NAME_ENTRY_WIDTH (DIR_LIST_WIDTH - 100)
#define NAME_ENTRY_HEIGHT 37

SDL_FRect nameEntryRect = {
	.x = NAME_ENTRY_X,
	.y = NAME_ENTRY_Y,
	.w = NAME_ENTRY_WIDTH,
	.h = NAME_ENTRY_HEIGHT,
};

SDL_Rect nameEntryInputRect = {
	.x = NAME_ENTRY_X,
	.y = NAME_ENTRY_Y,
	.w = NAME_ENTRY_WIDTH,
	.h = NAME_ENTRY_HEIGHT,
};

#define SAVE_BUTTON_X NAME_ENTRY_X + NAME_ENTRY_WIDTH + 5
#define SAVE_BUTTON_Y NAME_ENTRY_Y
#define SAVE_BUTTON_WIDTH DIR_LIST_WIDTH - NAME_ENTRY_WIDTH - 5
#define SAVE_BUTTON_HEIGHT NAME_ENTRY_HEIGHT

uiButton* saveButton;

bool endsWith(const char* str1, char* str2) {
	return (strcmp(str1+strlen(str1)-strlen(str2), str2) == 0);
}

void saveButtonCallback(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	char fullPath[MAX_DIR_ENTRY_LEN];
	sprintf(fullPath, "%s/%s", currentDir, fileName);
	printf("%s\n", fullPath);
	canvasData* canvas = getCanvas();
	// remove the pitch used in SDL's textures so it can be saved by stb_image_write
	uint32_t* rawPixels = malloc(sizeof(uint32_t) * canvas->w * canvas->h);

	uint32_t* p1 = rawPixels;
	uint32_t* p2 = canvas->pixels;
	for(uint16_t y = 0; y < canvas->h; ++y) {
		memcpy(p1, p2, sizeof(uint32_t) * canvas->w);
		p1 += canvas->w;
		p2 = (uint32_t*)((uint8_t*)p2 + canvas->pitch);
	}

	// endianness coming back to haunt me once again
	for(uint32_t i = 0; i < canvas->w * canvas->h; ++i) {
		rawPixels[i] = ((rawPixels[i] >> 24)&0xff) | ((rawPixels[i] >> 8)&0xff00) | ((rawPixels[i] << 8)&0xff0000) | ((rawPixels[i] << 24)&0xff000000);
	}


	bool saveFailed = false;
	if(endsWith(fileName, ".png")) {
		saveFailed = !stbi_write_png(fileName, canvas->w, canvas->h, 4, rawPixels, 0);
	} else if(endsWith(fileName, ".bmp")) {
		saveFailed = !stbi_write_bmp(fileName, canvas->w, canvas->h, 4, rawPixels);
	} else if(endsWith(fileName, ".jpg")) {
		saveFailed = !stbi_write_jpg(fileName, canvas->w, canvas->h, 4, rawPixels, 100);
	} else {
		saveFailed = true;
	}

	// would prefer to have this also in the actual ui once it's more complete
	if(saveFailed) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "save error", "could not save your image with that file type", NULL);
	} else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "save complete!", "save complete!", NULL);
	}
	free(rawPixels);
}

int dirCmp(const void* a, const void* b) {
	return strcmp((const char*)a, (const char*)b);
}

SDL_EnumerationResult addDirEntry(void* userdata, const char* dirname, const char* filename) {
	strcpy(dirEntries[currentDirSize], filename);
	++currentDirSize;
	return SDL_ENUM_CONTINUE;
}

void loadDir(char* path) {
	currentDirSize = 0;
	addDirEntry(NULL, NULL, ".");
	addDirEntry(NULL, NULL, "..");
	SDL_EnumerateDirectory(path, addDirEntry, NULL);
	dirEntries[currentDirSize][0] = '\0';
	qsort(dirEntries, currentDirSize, MAX_DIR_ENTRY_LEN, dirCmp);

	strcpy(currentDir, path);

	selectedEntry = 0xffffffff;
	scrollPosition = 0;
}

void saveModeInit(SDL_Renderer* renderer) {
	loadDir("./");

	SDL_SetTextInputArea(window, &nameEntryInputRect, 0);

	// update canvas texture to not have the brush preview on it
	lockCanvasTexture();
	unlockCanvasTexture();

	saveButton = createButton(SAVE_BUTTON_X, SAVE_BUTTON_Y, SAVE_BUTTON_WIDTH, SAVE_BUTTON_HEIGHT, NULL, 0x65a7f8ff, saveButtonCallback);
}

void saveModeUninit(void) {
	resetUI();
}

void saveModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	switch(e->type) {
		case SDL_EVENT_QUIT:
			running = false;
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			if((e->wheel.y < 0 && currentDirSize*37 + scrollPosition > DIR_LIST_HEIGHT) || (e->wheel.y > 0 && scrollPosition < 0)) {
				scrollPosition += e->wheel.y * 25;
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if(e->button.y > DIR_LIST_Y && e->button.y < DIR_LIST_Y + DIR_LIST_HEIGHT) {
				selectedEntry = (e->button.y - DIR_LIST_Y - scrollPosition) / 37;
				if(dirEntries[selectedEntry][0] == '\0') {
					selectedEntry = 0xffffffff;
				}
			}
			if(e->button.y > NAME_ENTRY_Y && e->button.y < NAME_ENTRY_Y + NAME_ENTRY_HEIGHT && e->button.x > NAME_ENTRY_X && e->button.x < NAME_ENTRY_WIDTH) {
				selectedEntry = 0xffffffff;
				SDL_StartTextInput(window);
				enteringName = true;
			} else {
				SDL_StopTextInput(window);
				enteringName = false;
			}
			break;
		case SDL_EVENT_KEY_DOWN:
			switch(e->key.key) {
				case SDLK_RETURN:
					if(selectedEntry != 0xffffffff) {
						char newDir[MAX_DIR_ENTRY_LEN];
						sprintf(newDir, "%s/%s", currentDir, dirEntries[selectedEntry]);
						loadDir(newDir);
						selectedEntry = 0xffffffff;
					} else if(enteringName){
						// saves the image, maybe should change the function name if I'm going to use it like this
						saveButtonCallback(mousePosX, mousePosY, mouseButtons);
					}
					break;
				case SDLK_BACKSPACE:
					fileName[strlen(fileName)-1] = '\0';
					break;
				case SDLK_ESCAPE:
					modeSwitch(&paintMode, renderer);
					return;
			}
			break;
		case SDL_EVENT_TEXT_INPUT:
			SDL_strlcat(fileName, e->text.text, sizeof(fileName));
			break;
		default:
			break;
	}

	SDL_SetRenderDrawColor(renderer, 48, 60, 75, 255);
	SDL_RenderClear(renderer);
	drawText(renderer, "gaming", 0,SCREEN_HEIGHT-32,2.0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xa0);
	SDL_SetRenderViewport(renderer, &dirListRect);
	SDL_RenderFillRect(renderer, NULL);

	if(selectedEntry != 0xffffffff) {
		SDL_SetRenderDrawColor(renderer, 28, 100, 180, 0xa0);
		SDL_RenderFillRect(renderer, &(SDL_FRect){.x = 2, .y = selectedEntry*37 + 2 + scrollPosition, .w = DIR_LIST_WIDTH - 44, .h = 34});
	}

	float y = 5 + scrollPosition;
	for(uint32_t i = 0; dirEntries[i][0] != '\0'; ++i) {
		drawText(renderer, dirEntries[i], 5,y,2.0);
		y+=32 + 5;
	}
	SDL_SetRenderViewport(renderer, NULL);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xa0);
	SDL_RenderFillRect(renderer, &nameEntryRect);
	drawText(renderer, fileName, NAME_ENTRY_X + 2, NAME_ENTRY_Y + 2, 2.0);

	if(enteringName) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &(SDL_FRect){NAME_ENTRY_X+2 + strlen(fileName)*16, NAME_ENTRY_Y, 4, 32});
	}

	drawCanvas(renderer, &canvasDisplayRect, NULL);

	drawUI(renderer);
	updateUI(e, mousePosX, mousePosY, mouseButtons);
	drawText(renderer, "save!", SAVE_BUTTON_X+2, SAVE_BUTTON_Y+2, 2.0);
}


