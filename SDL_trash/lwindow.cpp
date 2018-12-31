#include "lwindow.h"


LWindow::LWindow()
{
	// Init non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

LWindow::~LWindow() {}

bool LWindow::init(const char *title, int screen_width, int screen_height, bool shown)
{
	// Create Window 
	if (shown) {
		mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			screen_width, screen_height,
			SDL_WINDOW_SHOWN);
	} else {
		mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			screen_width, screen_height,
			SDL_WINDOW_HIDDEN);
	}
	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = screen_width;
		mHeight = screen_height;


		// Create renderer for window
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (mRenderer == NULL) {
			printf("Renderer could not be created. SDL ERROR: %s\n", SDL_GetError());
			SDL_DestroyWindow(mWindow);
			mWindow = NULL;
		}
		else {
			// Init renderer color
			SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			// Grab window indentifier
			mWindowID = SDL_GetWindowID(mWindow);

			// Set shown status
			mShown = shown;
		}
	}
	else {
		printf("Window could not be created. SDL ERROR: %s\n", SDL_GetError());
	}
	return (mWindow != NULL && mRenderer != NULL);
}

SDL_Renderer *LWindow::createRenderer()
{
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event &e) 
{
	// Window event occured
	if (e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID) {
		// Caption update flag
		bool updateCaption = false;
		switch (e.window.event) {
		// Window appeared
		case SDL_WINDOWEVENT_SHOWN:
			mShown = true;
			break;

		// Window disappeared
		case SDL_WINDOWEVENT_HIDDEN:
			mShown = false;
			break;

		// Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(mRenderer);
			break;

		// Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(mRenderer);
			break;

		// Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

		// Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

		// Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

		// Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

		// Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

		// Window maximized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

		// Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;

		// Hide on close
		case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow(mWindow);
			break;
		}
		// TODO implement caption update

		// Toggle fullscreen using f11
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
			if (mFullScreen) {
				SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
				mFullScreen = false;
			}
			else {
				SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
				mFullScreen = true;
				mMinimized = false;
			}
		}
	}
}

void LWindow::showWindow()
{
	if (!mShown) SDL_ShowWindow(mWindow);
}

void LWindow::hideWindow()
{
	if (mShown) SDL_HideWindow(mWindow);
}

void LWindow::focus()
{
	// Restore window if needed
	if (!mShown) SDL_ShowWindow(mWindow);

	// Move window forward
	SDL_RaiseWindow(mWindow);
}

void LWindow::renderText(LTexture *RenderTextTexture, TTF_Font *gFont, SDL_Color *textColor,
						std::stringstream *text)  
{
	if (!mMinimized && RenderTextTexture != NULL) {
		// Render text
		if (!RenderTextTexture->loadFromRenderedText(text->str().c_str(), *textColor,
			gFont, mRenderer)) {
			printf("Unable to render texture\n");
		}
	}
}

void LWindow::renderClear()
{
	// Clear screen
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mRenderer);
}

void LWindow::renderTexture(LTexture *RenderTexture, int width, int height, 
							SDL_Rect *clip, double degrees, SDL_Point *center, 
							SDL_RendererFlip flip)
{
	if (!mMinimized) {
		// Render Text Textures
		RenderTexture->render(width, height, mRenderer, clip, degrees, center, flip);
	}
}

void LWindow::renderUpdate()
{
	SDL_RenderPresent(mRenderer);
}

void LWindow::free() 
{
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	mRenderer = NULL;
	mWindow = NULL;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

SDL_Window *LWindow::getWindow()
{
	return mWindow;
}

SDL_Renderer *LWindow::getRenderer()
{
	return mRenderer;
}

void LWindow::setRenderer(SDL_Renderer *rend)
{
	mRenderer = rend;
}

bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
	return mMinimized;
}

bool LWindow::isShown()
{
	return mShown;
}

