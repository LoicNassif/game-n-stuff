#include "tile.h"



Tile::Tile(int x, int y, int tileType, int tile_width, int tile_height)
{
	// Get the offsets
	mBox.x = x;
	mBox.y = y;

	// Set the collision box
	mBox.w = tile_width;
	mBox.h = tile_height;

	// Get the tile type
	mType = tileType;
}


Tile::~Tile()
{
}

void Tile::render(LTexture *TileTexture, LWindow *targetWindow, SDL_Rect *spriteSheet)
{
	TileTexture->render(mBox.x, mBox.y, targetWindow->getRenderer(), &spriteSheet[mType]);
}

int Tile::getType()
{
	return mType;
}

SDL_Rect Tile::getBox()
{
	return mBox;
}