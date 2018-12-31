#include "LTexture.h"
#include <SDL_image.h>

LTexture::LTexture()
{
	// Init
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}


LTexture::~LTexture()
{
	// Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path, LWindow *targetWindow, bool colorKey) 
{
	// Get rid of preexisting texture
	free();

	// The final texture
	SDL_Texture *newTexture = NULL;

	// Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s SDL_image ERROR: %s\n", path.c_str(), IMG_GetError());
		return false;
	}
	else {
		// Colour key image (white)
		if (colorKey) {
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 255, 255));
		}
	}
	
	// Convert surface to display format
	SDL_Surface *formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface,
		SDL_GetWindowPixelFormat(targetWindow->getWindow()), NULL);

	if (formattedSurface == NULL) {
		printf("Unable to convert loaded surface to display format. SDL ERROR: %s\n", SDL_GetError());
		return false;
	}
	if (colorKey) {
		SDL_SetColorKey(formattedSurface, SDL_TRUE, SDL_MapRGB(formattedSurface->format, 255, 255, 255));
	}

	// Create blank streamable texture
	newTexture = SDL_CreateTexture(targetWindow->getRenderer(), SDL_GetWindowPixelFormat(targetWindow->getWindow()),
		SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
	if (newTexture == NULL) {
		printf("Unable to create blank texture. SDL ERROR: %s\n", SDL_GetError());
	}

	// Lock texture for manipulation
	SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

	// Copy loaded/formatted surface pixels
	memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

	// Unlock texture to update
	SDL_UnlockTexture(newTexture);
	mPixels = NULL;

	// Get image dimensions
	mWidth = formattedSurface->w;
	mHeight = formattedSurface->h;
	
	// Get rid of the old formatted surface
	SDL_FreeSurface(formattedSurface);

	// Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, 
									TTF_Font *gFont, SDL_Renderer *gRenderer)
{
	// Get rid of pre-existing texture
	free();

	// Render text surface
	SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL) {
		printf("Unable to render text surface. SDL_ttf ERROR: %s\n", TTF_GetError());
	}
	else {
		// Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text. SDL ERROR: %s\n", SDL_GetError());
		}
		else {
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Get rid of the old surface
		SDL_FreeSurface(textSurface);
	}

	return mTexture != NULL;
}
#endif

void LTexture::free() 
{
	// Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) 
{
	// Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	// Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	// Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Renderer *gRenderer, SDL_Rect *clip,
						double angle, SDL_Point *center, SDL_RendererFlip flip)
{
	// Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() 
{
	return mWidth;
}

int LTexture::getHeight() 
{
	return mHeight;
}

bool LTexture::lockTexture()
{
	// Texture is already locked
	if (mPixels != NULL) {
		printf("Texture is already locked\n");
		return false;
	}

	// Lock texture
	if (SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0) {
		printf("Unable to lock texture. %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool LTexture::unlockTexture()
{
	// Texture is already not locked
	if (mPixels == NULL) {
		printf("Texture is already not locked\n");
		return false;
	}

	// Unlock texture
	SDL_UnlockTexture(mTexture);
	mPixels = NULL;
	mPitch = 0;
	return true;
}

void *LTexture::getPixels() 
{
	return mPixels;
}

int LTexture::getPitch()
{
	return mPitch;
}
