#include "particle.h"

Particle::Particle(int x, int y, std::vector<LTexture> *textures)
{
	// Set offsets
	mPosX = x - 5 + (rand() % 25);
	mPosY = y - 5 + (rand() % 25);

	// Init animation
	mFrame = rand() % 5;

	// Set type
	switch (rand() % 3) {
	case 0:
		mTexture = &(*textures)[0];
		break;
	case 1:
		mTexture = &(*textures)[1];
		break;
	case 2:
		mTexture = &(*textures)[2];
		break;
	}
}


Particle::~Particle()
{
}

void Particle::render(SDL_Renderer *gRenderer, LTexture *ShimmerTexture)
{
	// Show image
	mTexture->render(mPosX, mPosY, gRenderer);

	// Show shimmer
	if (mFrame % 2 == 0) {
		ShimmerTexture->render(mPosX, mPosY, gRenderer);
	}

	// Animate
	++mFrame;
}

bool Particle::isDead()
{
	return mFrame > 15;
}