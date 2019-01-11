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
const int TOTAL_LEVELS = 8;

// Total number of destroys
const int TOTAL_DESTROY = 4;

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
const int TOTAL_BUTTONS = 2;
const int BUTTON_SPRITE_TOTAL = 2;
 
// Start time
const int TIME_START = 25;
const double TIME_SCALE = 1.5;

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
LTexture gBackgroundTexture;

// Tutorial texture
LTexture gTutorialTexture;

// Particle textures
LTexture gParticleGreen;
LTexture gParticleBlue;
LTexture gParticleRed;
LTexture gShimmerTexture;

// Game over texture
LTexture gGameOverTexture;

// Win over texture
LTexture gWinOverTexture;

// Tiles texture
LTexture gTileTexture;
// Lvl 1
std::vector<Tile *> tiles1;
SDL_Rect gTileClips1[TOTAL_TILE_SPRITES];
// Lvl 2
std::vector<Tile *> tiles2;
SDL_Rect gTileClips2[TOTAL_TILE_SPRITES];
// Lvl 3
std::vector<Tile *> tiles3;
SDL_Rect gTileClips3[TOTAL_TILE_SPRITES];
// Lvl 4
std::vector<Tile *> tiles4;
SDL_Rect gTileClips4[TOTAL_TILE_SPRITES];
// Lvl 5
std::vector<Tile *> tiles5;
SDL_Rect gTileClips5[TOTAL_TILE_SPRITES];
// Lvl 6
std::vector<Tile *> tiles6;
SDL_Rect gTileClips6[TOTAL_TILE_SPRITES];
// Lvl 7
std::vector<Tile *> tiles7;
SDL_Rect gTileClips7[TOTAL_TILE_SPRITES];
// Lvl 8
std::vector<Tile *> tiles8;
SDL_Rect gTileClips8[TOTAL_TILE_SPRITES];


// Map to keep array tiles in order
std::vector<LTexture *> texture_map;
std::vector<std::vector<Tile *> *> tiles_map;
std::vector<SDL_Rect *> tile_clips_map;

// Dot texture
LTexture gDotTexture;
LTexture gDot2Texture;

// Modulate test texture
LTexture gModulateTexture;

// FPS text
LTexture gFPSTextTexture;

// Timer texture
LTexture gTimerTextTexture;

// Score texture
LTexture gScoreTextTexture;

// Transition texture
LTexture transTextTexture;

// Start Button sprites
SDL_Rect gButtonStartClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonStartTexture;

// Restart Button sprites
SDL_Rect gButtonRestartClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonRestartTexture;

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
	if (!gWindow[0].init("Oof Ouch Owie", SCREEN_WIDTH, SCREEN_HEIGHT, true)) {
		printf("Window could not be created. SDL ERROR: %s\n", SDL_GetError());
		return false;
	}

	// Create lvls Windows
	for (int i = 1; i < TOTAL_WINDOWS; ++i) {
		if (!gWindow[i].init("My Bones", SCREEN_WIDTH, SCREEN_HEIGHT, false)) {
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
	gMusic = Mix_LoadMUS("assets\\music\\menu_song.mp3");
	if (gMusic == NULL) {
		printf("failed to load lavender town music. SDL_mixer ERROR: %s\n", Mix_GetError());
		return false;
	}

	// Load loading texture
	if (!gLoadTexture.loadFromFile("assets\\loading.png", &gWindow[0])) {
		printf("failed to load loading screen\n");
		return false;
	}

	// Load game over texture
	if (!gGameOverTexture.loadFromFile("assets\\game_over.png", &gWindow[1])) {
		printf("failed to load game over screen\n");
		return false;
	}

	// Load win over texture
	if (!gWinOverTexture.loadFromFile("assets\\win_over.png", &gWindow[1])) {
		printf("failed to load win over screen\n");
		return false;
	}

	// Load tutorial texture
	if (!gTutorialTexture.loadFromFile("assets\\controls_screen.png", &gWindow[1])) {
		printf("failed to load control screen\n");
		return false;
	}

	// Load tile texture lvls
	if (!gTileTexture.loadFromFile("assets\\tiles.png", &gWindow[1])) {
		printf("failed to load tiles textures\n");
		return false;
	}

	for (int i = 0; i < TOTAL_LEVELS; i++) {
		texture_map.push_back(&gTileTexture);
	}
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
	if (!setTiles(&tiles3, gTileClips3, "assets\\maps\\map3.map")) {
		printf("failed to load tile set 3\n");
		return false;
	}
	if (!setTiles(&tiles4, gTileClips4, "assets\\maps\\map4.map")) {
		printf("failed to load tile set 4\n");
		return false;
	}
	if (!setTiles(&tiles5, gTileClips5, "assets\\maps\\map5.map")) {
		printf("failed to load tile set 5\n");
		return false;
	}
	if (!setTiles(&tiles6, gTileClips6, "assets\\maps\\map6.map")) {
		printf("failed to load tile set 6\n");
		return false;
	}
	if (!setTiles(&tiles7, gTileClips7, "assets\\maps\\map7.map")) {
		printf("failed to load tile set 7\n");
		return false;
	}
	if (!setTiles(&tiles8, gTileClips8, "assets\\maps\\map8.map")) {
		printf("failed to load tile set 8\n");
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
	if (!gTextTexture.loadFromRenderedText("bffs", textColor, gFont, gWindow[0].getRenderer())) {
		printf("failed to render text texture\n");
		return false;
	}

	// Load Dot sprite
	if (!gDotTexture.loadFromFile("assets\\dot.png", &gWindow[1])) {
		printf("Failed to load dot sprite texture\n");
		return false;
	}
	// Load Dot sprite
	if (!gDot2Texture.loadFromFile("assets\\dot2.png", &gWindow[1])) {
		printf("Failed to load dot sprite texture\n");
		return false;
	}

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
	// Load start button
	if (!gButtonStartTexture.loadFromFile("assets\\start_button.png", &gWindow[0])) {
		printf("Failed to load start button sprite texture\n");
		return false;
	}

	// Set start sprites
	for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i) {
		gButtonStartClips[i].x = 0;
		gButtonStartClips[i].y = i * BUTTON_HEIGHT;
		gButtonStartClips[i].w = BUTTON_WIDTH;
		gButtonStartClips[i].h = BUTTON_HEIGHT;
 	}

	// Set the start button
	gButton[0].setPosition((SCREEN_WIDTH - BUTTON_WIDTH) / 2,
							(SCREEN_HEIGHT - BUTTON_HEIGHT) / 2 - 140);

	// Load restart button
	if (!gButtonRestartTexture.loadFromFile("assets\\restart_button.png", &gWindow[1])) {
		printf("Failed to load restart button sprite texture\n");
		return false;
	}

	// Set restart sprites
	for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i) {
		gButtonRestartClips[i].x = 0;
		gButtonRestartClips[i].y = 2 * i * BUTTON_HEIGHT;
		gButtonRestartClips[i].w = 2 * BUTTON_WIDTH;
		gButtonRestartClips[i].h = 2 * BUTTON_HEIGHT;
	}

	// Redefine dimensions
	gButton[1].setWidth(2 * BUTTON_WIDTH);
	gButton[1].setHeight(2 * BUTTON_HEIGHT);

	// Set the restart button
	gButton[1].setPosition((SCREEN_WIDTH - 2 * BUTTON_WIDTH) / 2,
							(SCREEN_HEIGHT - 2 * BUTTON_HEIGHT) / 2);

	return true;
}

void close() {
	// Free loaded image
	gLoadTexture.free();
	gTextTexture.free();
	gBackgroundTexture.free();
	gParticleGreen.free();
	gParticleBlue.free();
	gParticleRed.free();
	gShimmerTexture.free();
	gGameOverTexture.free();
	gWinOverTexture.free();
	gTileTexture.free();
	gDotTexture.free();
	gDot2Texture.free();
	gModulateTexture.free();
	gFPSTextTexture.free();
	gTimerTextTexture.free();
	gScoreTextTexture.free();
	transTextTexture.free();
	gButtonRestartTexture.free();
	gButtonStartTexture.free();

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
	bool first_flag = true;

	// Handle event
	SDL_Event e;

	// Set text color
	SDL_Color textColor = { 0,0,0,255 };
	SDL_Color gameWindowTextColor = { 255, 255, 255, 255 };

	// Game score
	int score=0;

	// Frames per second timer
	LTimer fpsTimer;

	// Lvl timer
	LTimer lvlTimer;

	// Transition timer
	LTimer transTimer;

	// In memory text stream
	std::stringstream FPSText;
	std::stringstream TimerText;
	std::stringstream ScoreText;
	std::stringstream transText;

	transText.str("");
	transText << "Get Ready";

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
	int curr_lvl = -1;

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
			if (gButton[0].getInside()) { // start the game
				switch (e.type) {
				case SDL_MOUSEBUTTONDOWN:
					gWindow[1].showWindow();
					gWindow[0].hideWindow();
					break;
				}
			}

			if (gButton[1].getInside()) { // restart the game
				switch (e.type) {
				case SDL_MOUSEBUTTONDOWN:
					curr_lvl = 0;
					score = 0;
					first_flag = true;
					dot1.setPosition(0, 60);
					dot2.setPosition(SCREEN_WIDTH - 60, 60);
					lvlTimer.stop();
					dot1.setDestroyTokens(TOTAL_DESTROY);
					dot2.setDestroyTokens(TOTAL_DESTROY);

					dot1.setWin(false); dot2.setWin(false);
					dot1.setChosenOne(false); dot2.setChosenOne(false);

					gWindow[1].hideWindow();
					gWindow[0].showWindow();
					for (size_t i = 0; i < tiles_map.size(); i++) {
						tiles_map[i]->clear();
					}
					tiles_map.clear();
					texture_map.clear();
					tile_clips_map.clear();
					// Load the Media
					if (!loadMedia()) {
						printf("Failed to load media\n");
						return 1;
					}
				}
			}
			
			// If in game screen get dot inputs
			if (gWindow[1].isShown()) {
				// User request exit
				if (e.type == SDL_QUIT) flag = true;

				// Handle dot input
				dot1.handleEvent(e, true);
				dot2.handleEvent(e, false);

				// handle skipping tutorial
				if (curr_lvl == -1) {
					if (e.type == SDL_KEYDOWN) {
						switch (e.key.keysym.sym) {
						case SDLK_SPACE:
							curr_lvl++;
							break;
						}
					}
				}

				// Handle destruction
				if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
					case SDLK_p:
						dot1.destroyBlock(tiles_map[curr_lvl]);
						break;
					case SDLK_q:
						dot2.destroyBlock(tiles_map[curr_lvl]);
						break;
					}
				}
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
			//gWindow[0].renderText(&gFPSTextTexture, FPSFont, &textColor, &FPSText);

			// Clear screens
			gWindow[0].renderClear();

			// Modulate texture
			gBackgroundTexture.setColor(r, g, b);
			gBackgroundTexture.setAlpha(a);

			// Render current frame
			gWindow[0].renderTexture(&gBackgroundTexture, 0, 0);
			gWindow[0].renderTexture(&gTextTexture, (SCREEN_WIDTH - gTextTexture.getWidth()) / 2,
				(SCREEN_HEIGHT - gTextTexture.getHeight()) / 2 - 170);

			gButton[0].render(&gButtonStartTexture, gWindow[0].getRenderer(),
					gButtonStartClips);
			
			//gWindow[0].renderTexture(&gFPSTextTexture, 0, 0);
		}
		else {
			// Play the music
			if (Mix_PlayingMusic() == 0) {
				// Play the music
				Mix_PlayMusic(gMusic, -1);
			}
			if (curr_lvl == -1) { // Quick tutorial screen
				gWindow[1].renderClear();
				gWindow[1].renderTexture(&gTutorialTexture, 0, 0);
			}
			if (curr_lvl == 0 && first_flag) {
				lvlTimer.start();
				first_flag = false;
				transText.str("");

				// Randomly choose the chosen one
				if (rand() % 2 > 0) {
					dot1.setChosenOne(true);
					transText << "Get Ready. p1 deletes";
				}
				else {
					dot2.setChosenOne(true);
					transText << "Get Ready. p2 deletes";
				}

				transTimer.start();
				while (transTimer.getTicks() / 1000.f < 2.5) {
					gWindow[1].renderClear();
					// Render the level
					for (int i = 0; i < TOTAL_TILES; i++) {
						(*tiles_map[curr_lvl])[i]->render(texture_map[curr_lvl], &gWindow[1], tile_clips_map[curr_lvl]);
					}

					// Render dot
					dot1.render(0, 0, &gDotTexture, &particleTextures, &gShimmerTexture, &gWindow[1]);
					dot2.render(0, 0, &gDot2Texture, &particleTextures, &gShimmerTexture, &gWindow[1]);

					// Render transition text
					gWindow[1].renderText(&transTextTexture, FPSFont, &gameWindowTextColor, &transText);
					gWindow[1].renderTexture(&transTextTexture, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

					gWindow[1].renderUpdate();
				}
				transTimer.stop();
			}

			// Set timer text to be rendered
			TimerText.str("");
			double time = (TIME_START - curr_lvl*TIME_SCALE) - lvlTimer.getTicks() / 1000.f;
			TimerText << time;

			if (time <= 0 && curr_lvl > -1) {
				lvlTimer.pause();
				// Clear screen
				gWindow[1].renderClear();
				// Render game over screen
				gWindow[1].renderTexture(&gGameOverTexture, 0, 0);
				// Display restart button
				gButton[1].render(&gButtonRestartTexture, gWindow[1].getRenderer(),
					gButtonRestartClips);
			}

			else if ((dot1.isWin() && dot2.isWin()) && -1 < curr_lvl && curr_lvl < TOTAL_LEVELS - 1) {
				score += (int)time * 100;
				ScoreText.str("");
				ScoreText << score;

				printf("lvl %d beat\n", curr_lvl);
				lvlTimer.stop();
				lvlTimer.start();
				curr_lvl++;
				dot1.setPosition(0, 60);
				dot2.setPosition(SCREEN_WIDTH - 60, 60);
				dot1.setDestroyTokens(TOTAL_DESTROY);
				dot2.setDestroyTokens(TOTAL_DESTROY);
				gWindow[1].renderClear();
				
				// Reset win state
				dot1.setWin(false); dot2.setWin(false);

				// Reset chosen state
				dot1.setChosenOne(false); dot2.setChosenOne(false);

				transText.str("");
				// Randomly choose the chosen one
				if (rand() % 2 > 0) {
					dot1.setChosenOne(true);
					transText << "Get Ready. p1 deletes";
				}
				else {
					dot2.setChosenOne(true);
					transText << "Get Ready. p2 deletes";
				}

				transTimer.start();
				while (transTimer.getTicks() / 1000.f < 2.5) {
					gWindow[1].renderClear();
					// Render the level
					for (int i = 0; i < TOTAL_TILES; i++) {
						(*tiles_map[curr_lvl])[i]->render(texture_map[curr_lvl], &gWindow[1], tile_clips_map[curr_lvl]);
					}

					// Render dot
					dot1.render(0, 0, &gDotTexture, &particleTextures, &gShimmerTexture, &gWindow[1]);
					dot2.render(0, 0, &gDot2Texture, &particleTextures, &gShimmerTexture, &gWindow[1]);
				
					// Render transition text
					gWindow[1].renderText(&transTextTexture, FPSFont, &gameWindowTextColor, &transText);
					gWindow[1].renderTexture(&transTextTexture, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

					gWindow[1].renderUpdate();
				}
				transTimer.stop();
			}
			else if ((dot1.isWin() && dot2.isWin()) && curr_lvl == TOTAL_LEVELS-1) {
				curr_lvl++;
				lvlTimer.stop();
				// Clear screen
				gWindow[1].renderClear();
				// Display Win over screen
				gWindow[1].renderTexture(&gWinOverTexture, 0, 0);
				// Render the score
				gWindow[1].renderText(&gScoreTextTexture, FPSFont, &textColor, &ScoreText);
				gWindow[1].renderTexture(&gScoreTextTexture, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100);
				// Render restart button
				gButton[1].render(&gButtonRestartTexture, gWindow[1].getRenderer(),
					gButtonRestartClips);
			}
			else if (time > 0 && curr_lvl < TOTAL_LEVELS && -1 < curr_lvl) {
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
				if (!dot1.isWin()) {
					dot1.render(0, 0, &gDotTexture, &particleTextures, &gShimmerTexture, &gWindow[1]);
				}
				else {
					dot1.setPosition(-10, -10);
				}
				if (!dot2.isWin()) {
					dot2.render(0, 0, &gDot2Texture, &particleTextures, &gShimmerTexture, &gWindow[1]);
				}
				else {
					dot2.setPosition(-10, -10);
				}
			}
			if (time > 0 && curr_lvl < TOTAL_LEVELS && -1 < curr_lvl) {
				// Render the timer
				gWindow[1].renderText(&gTimerTextTexture, FPSFont, &gameWindowTextColor, &TimerText);
				gWindow[1].renderTexture(&gTimerTextTexture, SCREEN_WIDTH / 2, 0);

				// Render the FPS
				//gWindow[1].renderText(&gFPSTextTexture, FPSFont, &gameWindowTextColor, &FPSText);
				//gWindow[1].renderTexture(&gFPSTextTexture, 0, 0);
			}
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