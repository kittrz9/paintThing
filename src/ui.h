#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

enum uiSliderOrientation {
	UI_SLIDER_HORIZONTAL,
	UI_SLIDER_VERTICAL,
};

typedef struct {
	float x;
	float y;

	float size;
	float value;

	enum uiSliderOrientation orientation; // unused for now, currently only using vertical sliders
} uiSlider;

void drawSliders(SDL_Renderer* renderer);

void updateSliders(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

uiSlider* createSlider(float x, float y, float size);
void destroySlider(uiSlider* slider);



void loadFont(SDL_Renderer* renderer, char* fontPath);
void drawText(SDL_Renderer* renderer, char* str, float x, float y, float scale);
void unloadFont(void);

#endif // UI_H
