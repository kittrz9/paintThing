#include "saveMode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define __USE_MISC // so DT_TYPE is defined
#include <dirent.h>

#include "ui.h"

#define DIR_LIST_X 20
#define DIR_LIST_Y 20
#define DIR_LIST_WIDTH SCREEN_WIDTH - 40
#define DIR_LIST_HEIGHT 500

SDL_Rect dirListRect = {
	.x = DIR_LIST_X,
	.y = DIR_LIST_Y,
	.w = DIR_LIST_WIDTH,
	.h = DIR_LIST_HEIGHT,
};

#define MAX_DIR_SIZE 256
#define MAX_DIR_ENTRY_LEN 128

char dirEntries[MAX_DIR_SIZE][MAX_DIR_ENTRY_LEN];
uint32_t currentDirSize;

uint32_t selectedEntry;

char currentDir[];

float scrollPosition;

int dirCmp(const void* a, const void* b) {
	return strcmp((const char*)a, (const char*)b);
}

void loadDir(char* path) {
	printf("%s\n", realpath(path, currentDir));
	DIR* d = opendir(path);

	if(d == NULL) {
		printf("could not open dir \"%s\"\n", path);
		return;
	}

	for(currentDirSize = 0; currentDirSize < MAX_DIR_SIZE; ++currentDirSize) {
		struct dirent* dir = readdir(d);
		if(dir == NULL) {
			break;
		}
		strcpy(dirEntries[currentDirSize], dir->d_name);
		if(dir->d_type == DT_DIR) {
			strcat(dirEntries[currentDirSize], "/");
		}
	}
	dirEntries[currentDirSize][0] = '\0';
	qsort(dirEntries, currentDirSize, MAX_DIR_ENTRY_LEN, dirCmp);
	closedir(d);

	strcpy(currentDir, path);
}

void saveModeInit(SDL_Renderer* renderer) {
	loadDir(".");
}

void saveModeUninit(void) {
}

void saveModeRun(SDL_Renderer* renderer, SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons) {
	switch(e->type) {
		case SDL_EVENT_QUIT:
			running = false;
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			if((e->wheel.y < 0 && currentDirSize*37 + scrollPosition > DIR_LIST_HEIGHT) || (e->wheel.y > 0 && scrollPosition < 0)) {
				scrollPosition += e->wheel.y * 5;
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if(e->button.y > DIR_LIST_Y && e->button.y < (currentDirSize+1)*37 - 25) {
				selectedEntry = (e->button.y - 25 - scrollPosition) / 37;
			}
			break;
		case SDL_EVENT_KEY_DOWN:
			if(e->key.key == SDLK_RETURN) {
				strcat(currentDir, "/");
				strcat(currentDir, dirEntries[selectedEntry]);
				loadDir(currentDir);
			}
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

	SDL_SetRenderDrawColor(renderer, 28, 100, 180, 0xa0);
	SDL_RenderFillRect(renderer, &(SDL_FRect){.x = 2, .y = selectedEntry*37 + 2 + scrollPosition, .w = DIR_LIST_WIDTH - 44, .h = 34});

	float y = 5 + scrollPosition;
	for(uint32_t i = 0; dirEntries[i][0] != '\0'; ++i) {
		drawText(renderer, dirEntries[i], 5,y,2.0);
		y+=32 + 5;
	}
	SDL_SetRenderViewport(renderer, NULL);
}


