#pragma once

#include <SDL.h>
#include <vector>
#include <ltexture.h>
#include "lwindow.h"
#include "particle.h"
#include "tile.h"

const int TOTAL_PARTICLES = 20;

// Circle struct
struct Circle {
	int x, y;
	int r;
};

class Dot
{
public:
	// Dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	// Maximum axis velocity of the dot
	static const int DOT_VEL = 4;

	Dot(int x, int y, std::vector<LTexture> *particleTexture);
	~Dot();

	// Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event &e, bool p1);

	// Moves the dot and check for collision
	void move(int xlim, int ylim, std::vector<SDL_Rect> *walls=NULL, std::vector<Circle> *circles=NULL,
				std::vector<SDL_Rect> *pixelColliders=NULL, std::vector<Tile *> *tileColliders=NULL);

	// Get the collision boxes
	std::vector<SDL_Rect> &getColliders();

	// Get circle collision
	Circle *getCircleCollider();

	// Shows the dot on the screen
	void render(int camX, int camY, LTexture *dotTexture,
				std::vector<LTexture> *particleTextures, LTexture *ShimmerTexture,
				LWindow *targetWindow);

	// Get positions
	int getPosX();
	int getPosY();

	// Set position
	void setPosition(int x, int y);

	// Get win status
	bool isWin();

	// Set win status
	void setWin(int status);

	// Destroy the block it is touching
	void destroyBlock(std::vector<Tile *> *TileColliders);

	// Set the amount of destroy tockens
	void setDestroyTokens(int param);

	// Set the chosen one param
	void setChosenOne(bool flag);

private:
	// The X and Y offsets of the dot
	int mPosX, mPosY;

	// Velocity of the dot
	int mVelX, mVelY;

	// Dot's collision box
	SDL_Rect mCollider;

	// Dot's destroy box
	SDL_Rect mColliderDestroy;

	// Dot's alternative circle box
	Circle *mCircleCollider = NULL;

	// Dot's per pixel collider
	std::vector<SDL_Rect> mPixelColliders;

	// Particles
	std::vector<Particle *> particles;

	// Move the collision boxes relative to the dot's offset
	void shiftColliders();

	// Show the particles
	void renderParticles(SDL_Renderer *gRenderer, std::vector<LTexture> *particleTextures,
						LTexture *ShimmerTexture);

	// flag to see if a dot won
	bool mWin=false;

	// Destroy tokens
	int mDestroy = 3;

	// Lock or unlock the ability to break blocks
	bool mChosenOne = false;
};

