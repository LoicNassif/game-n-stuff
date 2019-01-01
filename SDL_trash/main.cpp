#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ltexture.h>
#include <lbutton.h>
#include <vector>
#include "ltimer.h"
#include "lwindow.h"
#include "dot.h"

// Total windows
const int TOTAL_WINDOWS = 2;

// Screen dimensions
const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 600;

// Total number of tiles
const int TOTAL_TILES = 180;

// Total number of levels
const int TOTAL_LEVELS = 2;

// Total number of tile sprites
const int TOTAL_TILE_SPRITES = 4;

// Tiles dimensions
const int TILE_WIDTH = 40;
const int TILE_HEIGHT = 40;

// The different tile types
enum TileTypes {
	TILE_RED = 0,
	TILE_GREEN = 1,
	TILE_BLUE = 2,
	TILE_BLACK = 3
};

// Button constants
const int BUTTON_WIDTH = 50;
const int BUTTON_HEIGHT = 20;
const int TOTAL_BUTTONS = 1;
const int BUTTON_SPRITE_TOTAL = 2;
 
// Key press surface constants
enum KeyPressSurfaces {
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

// The windows we'll be rending to
LWindow gWindow[TOTAL_WINDOWS];

// Globally used fonts
TTF_Font *gFont = NULL;
TTF_Font *FPSFont = NULL;

// Loading texture
LTexture gLoadTexture;

// Rendered texture
LTexture gTextTexture;

// Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;

// Particle textures
LTexture gParticleGreen;
LTexture gParticleBlue;
LTexture gParticleRed;
LTexture gShimmerTexture;

// Tiles texture
// Lvl 1
LTexture gTileTexture1;
std::vector<Tile *> tiles1;
SDL_Rect gTileClips1[TOTAL_TILE_SPRITES];
// Lvl 2
LTexture gTileTexture2;
std::vector<Tile *> tiles2;
SDL_Rect gTileClips2[TOTAL_TILE_SPRITES];

// Map to keep array tiles in order
std::vector<LTexture *> texture_map;
std::vector<std::vector<Tile *> *> tiles_map;
std::vector<SDL_Rect *> tile_clips_map;

// Dot texture
LTexture gDotTexture1;
LTexture gDot2Texture1;
LTexture gDotTexture2;
LTexture gDot2Texture2;
std::vector<LTexture *> dot1_texture_map;
std::vector<LTexture *> dot2_texture_map;

// Modulate test texture
LTexture gModulateTexture;

// FPS text
LTexture gFPSTextTexture;

// Mouse Button sprites
SDL_Rect gButtonSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonSpriteSheetTexture;

// Button objects
std::vector<LButton> gButton(TOTAL_BUTTONS, LButton(BUTTON_WIDTH, BUTTON_HEIGHT));

// Music
Mix_Music *gMusic = NULL;

// Quick check if file is present in directory
inline bool file_exists(const std::string &filename) {
	FILE *file;
	if (fopen_s(&file, filename.c_str(), "r") == 0) {
		printf("file exists\n");
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

bool setTiles(std::vector<Tile *> *tiles, SDL_Rect *gTileClips, const char *lvl) {
	// Tile offset
	int x = 0, y = 0;

	// Open the map
	std::ifstream map(lvl);
	if (map.fail()) {
		printf("Unable to load map file\n");
		return false;
	}

	// Init the tiles
	for (int i = 0; i < TOTAL_TILES; i++) {
		// Determine what kind of tile will be made
		int tileType = -1;
		map >> tileType;

		// If there is a problem reading the file
		if (map.fail()) {
			// Stop loading map
			printf("Error loading map: Unexpected Error\n");
			return false;
		}

		// If the number is valid type
		if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES)) {
			tiles->push_back(new Tile(x, y, tileType, TILE_WIDTH, TILE_HEIGHT));
		}
		else {
			// Invalid tile type
			printf("Error loading map: Invalid tile type at %d\n", i);
			return false;
		}

		// Move to the next tile spot
		x += TILE_WIDTH;

		// If we've gone too far
		if (x >= SCREEN_WIDTH) {
			// Move back and next row
			x = 0;
			y += TILE_HEIGHT;
		}
	}
	// Set up the clips
	gTileClips[TILE_RED].x = 0; gTileClips[TILE_RED].y = 0;
	gTileClips[TILE_RED].w = TILE_WIDTH; gTileClips[TILE_RED].h = TILE_HEIGHT;

	gTileClips[TILE_GREEN].x = 0; gTileClips[TILE_GREEN].y = TILE_HEIGHT;
	gTileClips[TILE_GREEN].w = TILE_WIDTH; gTileClips[TILE_GREEN].h = TILE_HEIGHT;

	gTileClips[TILE_BLUE].x = TILE_WIDTH; gTileClips[TILE_BLUE].y = 0;
	gTileClips[TILE_BLUE].w = TILE_WIDTH; gTileClips[TILE_BLUE].h = TILE_HEIGHT;

	gTileClips[TILE_BLACK].x = TILE_WIDTH; gTileClips[TILE_BLACK].y = TILE_HEIGHT;
	gTileClips[TILE_BLACK].w = TILE_WIDTH; gTileClips[TILE_BLACK].h = TILE_HEIGHT;

	tiles_map.push_back(tiles);
	tile_clips_map.push_back(gTileClips);

	map.close();
	return true;
}

bool init() {
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL init fail: %s\n", SDL_GetError());
		return false;
	}

	// Create Start Window
	if (!gWindow[0].init("MyBones", SCREEN_WIDTH, SCREEN_HEIGHT, true)) {
		printf("Window could not be created. SDL ERROR: %s\n", SDL_GetError());
		return false;
	}

	// Create lvls Windows
	for (int i = 1; i < TOTAL_WINDOWS; ++i) {
		if (!gWindow[i].init("testing", SCREEN_WIDTH, SCREEN_HEIGHT, false)) {
			printf("Window could not be created. SDL ERROR: %s\n", SDL_GetError());
			return false;
		}
	}
	// Init PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags)&imgFlags)) {
		printf("SDL_image could not init SDL_image ERROR: %s\n", IMG_GetError());
		return false;
	}

	// Init SDL mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer could not be init. SDL_mixer ERROR: %s\n", Mix_GetError());
		return false;
	}

	// Init SDL_ttf
	if (TTF_Init() == -1) {
		printf("SDL_ttf could not be init. SDL_ttf ERROR: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

SDL_Texture* loadTexture(std::string path) {
	// The final texture
	SDL_Texture *newTexture = NULL;

	// Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s SDL ERROR: %s\n", path.c_str(), IMG_GetError());
	}
	else {
		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gWindow[0].getRenderer(), loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s. SDL ERROR: %s\n", path.c_str(), SDL_GetError());
		}

		// Get rid of the old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

bool loadMedia() {
	// Load music
	gMusic = Mix_LoadMUS("assets\\music\\lavender.mp3");
	if (gMusic == NULL) {
		printf("failed to load lavender town music. SDL_mixer ERROR: %s\n", Mix_GetError());
		return false;
	}

	// Load loading texture
	if (!gLoadTexture.loadFromFile("assets\\loading.png", &gWindow[0])) {
		printf("failed to load loading screen\n");
		return false;
	}

	// Load tile texture lvl 1
	if (!gTileTexture1.loadFromFile("assets\\tiles.png", &gWindow[1])) {
		printf("failed to load tiles textures\n");
		return false;
	}

	// Load tile texture lvl 2
	if (!gTileTexture2.loadFromFile("assets\\tiles.png", &gWindow[1])) {
		printf("failed to load tiles textures\n");
		return false;
	}

	texture_map.push_back(&gTileTexture1);
	texture_map.push_back(&gTileTexture2);

	// Display loading screen
	gWindow[0].renderClear();
	gWindow[0].renderTexture(&gLoadTexture, 0, 0);
	gWindow[0].renderUpdate();

	// Generate random maps
	if (!file_exists("assets\\maps\\map1.map")) {
		printf("map does not exists. Generating maps...\n");
		system("python assets\\maps\\generate_level.py");
	}
	// Load tile maps
	if (!setTiles(&tiles1, gTileClips1, "assets\\maps\\map1.map")) {
		printf("failed to load tile set 1\n");
		return false;
	}
	if (!setTiles(&tiles2, gTileClips2, "assets\\maps\\map2.map")) {
		printf("failed to load tile set 2\n");
		return false;
	}

	gWindow[0].renderClear();

	// Load modulation texture and load alpha capabilities
	if (!gModulateTexture.loadFromFile("assets\\background.png", &gWindow[0])) {
		printf("failed to load modulate texture image\n");
		return false;
	}

	// Set standard alpha blending
	gModulateTexture.setBlendMode(SDL_BLENDMODE_BLEND);

	// Load background texture
	if (!gBackgroundTexture.loadFromFile("assets\\background.png", &gWindow[0])) {
		printf("failed to load background texture image\n");
		return false;
	}

	// Open the fonts
	gFont = TTF_OpenFont("assets\\fonts\\AmaticSC-Regular.ttf", 28);
	if (gFont == NULL) {
		printf("failed to load AmaticSC-Regular font. SDL_ttf ERROR: %s\n", TTF_GetError());
		return false;
	}
	FPSFont = TTF_OpenFont("assets\\fonts\\BebasNeue-Regular.ttf", 28);
	if (FPSFont == NULL) {
		printf("failed to load BebasNeue-Regular font. SDL_ttf ERROR: %s\n", TTF_GetError());
		return false;
	}

	// Render text
	SDL_Color textColor = { 0,0,0 };
	SDL_Color gameWindowTextColor = { 255, 255, 255 };
	if (!gTextTexture.loadFromRenderedText("Oof Ouch Owie", textColor, gFont, gWindow[0].getRenderer())) {
		printf("failed to render text texture\n");
		return false;
	}

	// Load Dot sprite lvl 1
	if (!gDotTexture1.loadFromFile("assets\\dot.png", &gWindow[1])) {
		printf("Failed to load dot sprite texture\n");
		return false;
	}
	// Load Dot sprite lvl 1
	if (!gDot2Texture1.loadFromFile("assets\\dot2.png", &gWindow[1])) {
		printf("Failed to load dot sprite texture\n");
		return false;
	}

	// Load Dot sprite lvl 2
	if (!gDotTexture2.loadFromFile("assets\\dot.png", &gWindow[1])) {
		printf("Failed to load dot sprite texture\n");
		return false;
	}
	// Load Dot sprite lvl 2
	if (!gDot2Texture2.loadFromFile("assets\\dot2.png", &gWindow[1])) {
		printf("Failed to load dot sprite texture\n");
		return false;
	}

	dot1_texture_map.push_back(&gDotTexture1);
	dot1_texture_map.push_back(&gDotTexture2);
	dot2_texture_map.push_back(&gDot2Texture1);
	dot2_texture_map.push_back(&gDot2Texture2);

	// Load particles
	if (!gParticleBlue.loadFromFile("assets\\blue.bmp", &gWindow[1])) {
		printf("Failed to load blue particle texture\n");
		return false;
	}
	if (!gParticleGreen.loadFromFile("assets\\green.bmp", &gWindow[1])) {
		printf("Failed to load green particle texture\n");
		return false;
	}
	if (!gParticleRed.loadFromFile("assets\\red.bmp", &gWindow[1])) {
		printf("Failed to load red particle texture\n");
		return false;
	}
	if (!gShimmerTexture.loadFromFile("assets\\shimmer.bmp", &gWindow[1])) {
		printf("Failed to load shimmer particle texture\n");
		return false;
	}

	// Load button sprites
	if (!gButtonSpriteSheetTexture.loadFromFile("assets\\start_button.png", &gWindow[0])) {
		printf("Failed to load button sprite texture\n");
		return false;
	}

	else {
		// Set sprites
		for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i) {
			gButtonSpriteClips[i].x = 0;
			gButtonSpriteClips[i].y = i * 20;
			gButtonSpriteClips[i].w = BUTTON_WIDTH;
			gButtonSpriteClips[i].h = BUTTON_HEIGHT;
 		}

		// Set the button
		gButton[0].setPosition((SCREEN_WIDTH - BUTTON_WIDTH) / 2,
								(SCREEN_HEIGHT - BUTTON_HEIGHT) / 2 - 140);
	}

	return true;
}

void close() {
	// Free loaded image FREE MORE
	gFooTexture.free();
	gBackgroundTexture.free();
	gTextTexture.free();
	gButtonSpriteSheetTexture.free();
	gDotTexture1.free();

	// Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	// Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	// Destroy the window
	for (int i=0; i<TOTAL_WINDOWS; i++) {
		gWindow[i].free();
	}

	// Quit the SDL subsystem
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char *argv[]) {

	// Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize\n");
		return -1;
	}

	// Load the Media
	if (!loadMedia()) {
		printf("Failed to load media\n");
		return 1;
	}

	// Loop flag
	bool flag = false;

	// Handle event
	SDL_Event e;

	// Set text color
	SDL_Color textColor = { 0,0,0,255 };
	SDL_Color gameWindowTextColor = { 255, 255, 255, 255 };

	// Frames per second timer
	LTimer fpsTimer;

	// In memory text stream
	std::stringstream FPSText;

	// Start couting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	// Current animation frame
	int frame = 0;

	// Set up particles
	std::vector<LTexture> particleTextures;
	particleTextures.push_back(gParticleBlue);
	particleTextures.push_back(gParticleGreen);
	particleTextures.push_back(gParticleRed);

	// Dots
	Dot dot1(0, 60, &particleTextures);
	Dot dot2(SCREEN_WIDTH-60, 60, &particleTextures);

	// Modulation components
	Uint8 r = 255;
	Uint8 g = 255;
	Uint8 b = 255;
	Uint8 a = 255;

	// Starting lvl
	int curr_lvl = 0;

	while (!flag) {
		while (SDL_PollEvent(&e) != 0) {
			// Handle window events
			for (int i = 0; i < TOTAL_WINDOWS; ++i) {
				gWindow[i].handleEvent(e);
			}
			// Handle button events
			for (int i = 0; i < TOTAL_BUTTONS; ++i) {
				gButton[i].handleEvent(&e);
			}

			// Handle custom button events
			if (gButton[0].getInside()) {
				switch (e.type) {
				case SDL_MOUSEBUTTONDOWN:
					gWindow[1].showWindow();
					gWindow[0].hideWindow();
					break;
				}
			}

			// If in game screen get dot inputs
			if (gWindow[1].isShown()) {
				// User request exit
				if (e.type == SDL_QUIT) flag = true;
				
				// Handle dot input
				dot1.handleEvent(e, true);
				dot2.handleEvent(e, false);
			}

			// User request exit
			else if (e.type == SDL_QUIT) flag = true;		

			// On keypress change RGB values
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				// Increase red
				case SDLK_q:
					r += 20;
					break;

				// Increase green
				case SDLK_w:
					g += 20;
					break;

				// Increase blue
				case SDLK_e:
					b += 20;
					break;
				}
			}
		}
		// Compute frame rate
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (avgFPS > 2000000) avgFPS = 0;

		// Set FPS text to be rendered
		FPSText.str("");
		FPSText << avgFPS;

		// Main menu
		if (gWindow[0].isShown()) {
			// Play the music
			if (Mix_PlayingMusic() == 0) {
				// Play the music
				Mix_PlayMusic(gMusic, -1);
			}

			// Render text
			gWindow[0].renderText(&gFPSTextTexture, FPSFont, &textColor, &FPSText);

			// Clear screens
			for (int i = 0; i < TOTAL_WINDOWS; i++) gWindow[i].renderClear();

			// Modulate texture
			gBackgroundTexture.setColor(r, g, b);
			gBackgroundTexture.setAlpha(a);

			// Render current frame
			gWindow[0].renderTexture(&gBackgroundTexture, 0, 0);
			gWindow[0].renderTexture(&gTextTexture, (SCREEN_WIDTH - gTextTexture.getWidth()) / 2,
				(SCREEN_HEIGHT - gTextTexture.getHeight()) / 2 - 170);

			for (int i = 0; i < TOTAL_BUTTONS; i++) {
				gButton[i].render(&gButtonSpriteSheetTexture, gWindow[0].getRenderer(),
					gButtonSpriteClips);
			}
			gWindow[0].renderTexture(&gFPSTextTexture, 0, 0);
		}
		else {
			if ((dot1.isWin() || dot2.isWin()) && curr_lvl < TOTAL_LEVELS - 1) {
				curr_lvl++;
				dot1.setPosition(0, 60);
				dot2.setPosition(SCREEN_WIDTH - 60, 60);
				gWindow[1].renderClear();

				// Render the level
				for (int i = 0; i < TOTAL_TILES; i++) {
					(*tiles_map[curr_lvl])[i]->render(texture_map[curr_lvl], &gWindow[1], tile_clips_map[curr_lvl]);
				}

				// Render dot
				dot1.render(0, 0, dot1_texture_map[curr_lvl], &particleTextures, &gShimmerTexture, &gWindow[1]);
				dot2.render(0, 0, dot2_texture_map[curr_lvl], &particleTextures, &gShimmerTexture, &gWindow[1]);

			}
			else {
				// Move dot
				dot1.move(SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, &dot2.getColliders(),
					tiles_map[curr_lvl]);
				dot2.move(SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, &dot1.getColliders(),
					tiles_map[curr_lvl]);

				// Clear screens
				for (int i = 0; i < TOTAL_WINDOWS; i++) gWindow[i].renderClear();

				// Render the level
				for (int i = 0; i < TOTAL_TILES; i++) {
					(*tiles_map[curr_lvl])[i]->render(texture_map[curr_lvl], &gWindow[1], tile_clips_map[curr_lvl]);
				}

				// Render dot
				dot1.render(0, 0, dot1_texture_map[curr_lvl], &particleTextures, &gShimmerTexture, &gWindow[1]);
				dot2.render(0, 0, dot2_texture_map[curr_lvl], &particleTextures, &gShimmerTexture, &gWindow[1]);
			}

			// Render the FPS
			gWindow[1].renderText(&gFPSTextTexture, FPSFont, &gameWindowTextColor, &FPSText);
			gWindow[1].renderTexture(&gFPSTextTexture, 0, 0);
		}
		// Update All Screens
		for (int i = 0; i < TOTAL_WINDOWS; i++) gWindow[i].renderUpdate();

		// Check all windows
		bool allWindowsClosed = true;
		for (int i = 0; i < TOTAL_WINDOWS; i++) {
			if (gWindow[i].isShown()) {
				allWindowsClosed = false;
				break;
			}
		}

		// Application closed all windows
		if (allWindowsClosed) flag = true;

		// Go to the next frame
		++frame;
		++countedFrames;
	}

	// Free resources and close SDL
	close();

	return 0;
}