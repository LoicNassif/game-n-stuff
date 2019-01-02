#include "lbutton.h"


LButton::LButton(int button_width, int button_height)
{
	mPosition.x = 0;
	mPosition.y = 0;
	mInside = false;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
	BUTTON_WIDTH = button_width;
	BUTTON_HEIGHT = button_height;
}

LButton::~LButton() {}

void LButton::setPosition(int x, int y) 
{
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent(SDL_Event *e) 
{
	// If mouse event happened
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
		// Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		// Check if the mouse is in button
		mInside = true;

		// Mouse is left of the button
		if (x < mPosition.x) mInside = false;
		
		// Mouse is right of the button
		if (x > mPosition.x + BUTTON_WIDTH) mInside = false;
		
		// Mouse above the button
		if (y < mPosition.y) mInside = false;

		// Mouse below the button
		else if (y > mPosition.y + BUTTON_HEIGHT) mInside = false;

		// Mouse is outside button
		if (!mInside) {
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		else { // Mouse is inside button
			// Set mouse over sprite
			switch (e->type) {
			case SDL_MOUSEMOTION:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;

			case SDL_MOUSEBUTTONUP:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}
}

void LButton::render(LTexture *gButtonSpriteSheetTexture, SDL_Renderer *gRenderer, SDL_Rect *gSpriteClips)
{
	// Show current button sprite
	gButtonSpriteSheetTexture->render(mPosition.x, mPosition.y, gRenderer, &gSpriteClips[mCurrentSprite]);
}

bool LButton::getInside() 
{
	return mInside;
}

void LButton::setWidth(int w)
{
	BUTTON_WIDTH = w;
}

void LButton::setHeight(int h)
{
	BUTTON_HEIGHT = h;
}