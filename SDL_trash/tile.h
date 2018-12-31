#pragma once

#include <SDL.h>
#include <ltexture.h>
#include "lwindow.h"

class Tile
{
public:
	Tile(int x, int y, int tileType, int tile_width, int tile_height);
	~Tile();

	// Show the tile
	void render(LTexture *tileTexture, LWindow *targetWindow, SDL_Rect *spriteSheet);

	// Get the tile type
	int getType();

	// Get the collision box
	SDL_Rect getBox();

private:
	// The attributes of the tile
	SDL_Rect mBox;

	// The tile type
	int mType;
};

