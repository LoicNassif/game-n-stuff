#pragma once
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "lwindow.h"

/* A texture wrapper */
class LWindow;

class LTexture
{
public:
	LTexture();
	~LTexture();

	// Load image at specified path
	bool loadFromFile(std::string path, LWindow *targetWindow, bool colorKey=false);

#ifdef _SDL_TTF_H
	// Create image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor, 
								TTF_Font *gFont, SDL_Renderer *gRenderer);
#endif
	// Deallocates textures
	void free();

	// Set colour modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// Set blending
	void setBlendMode(SDL_BlendMode blending);

	// Set alpha modulation
	void setAlpha(Uint8 alpha);

	// Renders texture at given point
	void render(int x, int y, SDL_Renderer *gRenderer, SDL_Rect *clip = NULL, 
				double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip=SDL_FLIP_NONE);

	// Gets image dimensions
	int getWidth();
	int getHeight();

	// Pixel manipulators
	bool lockTexture();
	bool unlockTexture();
	void *getPixels();
	int getPitch();

private:
	// The actual hardware texture
	SDL_Texture *mTexture;
	void *mPixels;
	int mPitch;

	// Image dimensions
	int mWidth;
	int mHeight;
};

