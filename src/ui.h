#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>

#include <stdbool.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

extern SDL_Window* window;

void drawUI(SDL_Renderer* renderer);

void updateUI(SDL_Event* e, float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

void resetUI(void);

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

	bool allocated;
} uiSlider;

uiSlider* createSlider(float x, float y, float size);
void destroySlider(uiSlider* slider);

typedef struct {
	float x;
	float y;

	float w;
	float h;

	// unused for now
	SDL_Texture* texture;
	uint32_t color;

	// could probably use some va_arg shenanigans to pass whatever arguments to the callback specified when the button is created
	void (*clickCallback)(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons);

	bool allocated;
} uiButton;

uiButton* createButton(float x, float y, float w, float h, SDL_Texture* texture, uint32_t color, void (*clickCallback)(float mousePosX, float mousePosY, SDL_MouseButtonFlags mouseButtons));
void destroyButton(uiButton* button);

typedef struct {
	float x;
	float y;

	float size;

	bool* flag;

	bool allocated;
} uiCheckbox;

uiCheckbox* createCheckbox(float x, float y, float size, bool* flag);
void destroyCheckbox(uiCheckbox* checkbox);

void loadFont(SDL_Renderer* renderer);
void drawText(SDL_Renderer* renderer, char* str, float x, float y, float scale);
void unloadFont(void);

#endif // UI_H
