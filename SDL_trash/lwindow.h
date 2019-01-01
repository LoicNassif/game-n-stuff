#pragma once

#include <SDL.h>
#include <sstream>
#include <ltexture.h>
#include <SDL_ttf.h>
#include <vector>

class LTexture;

class LWindow
{
public:
	LWindow();
	~LWindow();

	// Create window
	bool init(const char *title, int screen_width, int screen_height, bool shown);

	// Create renderer from internal window
	SDL_Renderer *createRenderer();

	// Handles window events
	void handleEvent(SDL_Event &e);

	// Show window
	void showWindow();

	// Hide window
	void hideWindow();

	// Focus on window
	void focus();

	// Render window content
	void renderText(LTexture *RenderTextTexture, TTF_Font *gFont = NULL, SDL_Color *textColor = NULL,
		std::stringstream *text = NULL);
	void renderClear();
	void renderTexture(LTexture *RenderTexture, int width, int height,
		SDL_Rect *clip = NULL, double degrees = 0.0,
		SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void renderUpdate();

	// Deallocates internals
	void free();

	// Window dimensions
	int getWidth();
	int getHeight();

	// Get the window
	SDL_Window *getWindow();

	// Getter and setter window renderer
	SDL_Renderer *getRenderer();
	void setRenderer(SDL_Renderer *rend);

	// Window first init status
	bool isNew();
	void setNew(bool status);

	// Window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();
	bool isShown();

private:
	// Window data
	SDL_Window *mWindow;
	SDL_Renderer *mRenderer;
	int mWindowID;

	// Window dimensions
	int mWidth;
	int mHeight;

	// Window focii
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
	bool mShown;

	bool mNew = true;
};

