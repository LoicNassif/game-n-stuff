#pragma once

#include <ltexture.h>
#include <vector>

class Particle
{
public:
	Particle(int x, int y, std::vector<LTexture> *textures);
	~Particle();

	// render the particle
	void render(SDL_Renderer *gRenderer, LTexture *ShimmerTexture);

	// Check if particle is dead
	bool isDead();

private:
	// Offsets
	int mPosX, mPosY;

	// Current frame of animation
	int mFrame;

	// Type of particle
	LTexture *mTexture;
};

