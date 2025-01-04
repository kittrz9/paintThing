#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>

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

#endif // UI_H
