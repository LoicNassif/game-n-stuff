#pragma once
#include <SDL.h>
#include <ltexture.h>

// Mouse action surface constants
enum LButtonSprite {
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3, 
};

class LButton
{
public:
	LButton(int button_width, int button_height);
	~LButton();

	// Sets top left position
	void setPosition(int x, int y);

	// Handles mouse event
	void handleEvent(SDL_Event *e);

	// Shows button sprite
	void render(LTexture *gButtonTexture, SDL_Renderer *gRenderer, SDL_Rect *gSpriteClips);

	// Getter inside flag
	bool getInside();

private:
	// Top left position
	SDL_Point mPosition;

	// Currently used global sprite
	LButtonSprite mCurrentSprite;

	// Button dimensions
	int BUTTON_WIDTH;
	int BUTTON_HEIGHT;

	// In or out of the button
	bool mInside;
};

