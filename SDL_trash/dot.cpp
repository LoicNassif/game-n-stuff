#include "dot.h"

// Find distance squared
double distanceSquared(int x1, int y1, int x2, int y2) {
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	return deltaX * deltaX + deltaY * deltaY;
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	// The sides of the rectangles
	int leftA, rightA, topA, bottomA;
	int leftB, rightB, topB, bottomB;

	// Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	// Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	// If any of the sides from A are outside of B
	if (bottomA <= topB) return false;
	if (topA >= bottomB) return false;
	if (rightA <= leftB) return false;
	if (leftA >= rightB) return false;

	return true;
}

bool touchesVictory(SDL_Rect box, std::vector<Tile *> *tiles, int tile_victor)
{
	// Go through the tiles
	for (size_t i = 0; i < tiles->size(); ++i) {
		// If the tile is a victory tile
		if ((*tiles)[i]->getType() == tile_victor) {
			// If the collision box touches the victory tile
			if (checkCollision(box, (*tiles)[i]->getBox())) {
				return true; 
			}
		}
	}

	return false;
}

bool touchesWalls(SDL_Rect box, std::vector<Tile *> *tiles, int tile_center, int tile_topleft) {
	// Go through the tiles
	for (size_t i = 0; i < tiles->size(); ++i) {
		// If the tile is a wall type tile
		if (((*tiles)[i]->getType() < tile_center) && ((*tiles)[i]->getType() < tile_topleft)) {
			// If the collision box touches the wall tile
			if (checkCollision(box, (*tiles)[i]->getBox())) { 
				//printf("detected collision %d\n", i);
				return true; 
			}
		}
	}

	return false;
}

bool checkCollision(Circle &a, Circle &b) {
	// Calculate total radius squared
	int totalRadiusSquared = a.r + b.r;
	totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

	// If the distance between the centers of the circles is less than the sum of their radii
	if (distanceSquared(a.x, a.y, b.x, b.y) < totalRadiusSquared) {
		// Collision detected
		return true;
	}

	return false;
}

bool checkCollision(Circle &a, SDL_Rect &b) {
	// Closest point on collision box
	int cX, cY;

	// Find closest x offset
	if (a.x < b.x) cX = b.x;
	else if (a.x > b.x + b.w) cX = b.x + b.w;
	else cX = a.x;

	// Find the closest y offset
	if (a.y < b.y) cY = b.y;
	else if (a.y > b.y + b.h) cY = b.y + b.h;
	else cY = a.y;

	// If the closest point is in the circle
	if (distanceSquared(a.x, a.y, cX, cY) < a.r * a.r) {
		// Collision detected
		return true;
	}

	return false;
}

bool checkPixelCollision(std::vector<SDL_Rect> &a, std::vector<SDL_Rect> &b)
{
	// The sides of the rectangles
	int leftA, rightA, topA, bottomA;
	int leftB, rightB, topB, bottomB;

	// Go through the A boxes
	for (size_t Abox = 0; Abox < a.size(); Abox++) {
		// Calculate the sides of rect A
		leftA = a[Abox].x;
		rightA = a[Abox].x + a[Abox].w;
		topA = a[Abox].y;
		bottomA = a[Abox].y + a[Abox].h;

		// Go through the B boxes
		for (size_t Bbox = 0; Bbox < b.size(); Bbox++) {
			// Calculate the sides of rect B
			leftB = b[Bbox].x;
			rightB = b[Bbox].x + b[Bbox].w;
			topB = b[Bbox].y;
			bottomB = b[Bbox].y + b[Bbox].h;

			// If no sides from A are outside of B
			if (((bottomA <= topB) || (topA >= bottomB) || (rightA <= leftB) || (leftA >= rightB)) == false) {
				// Collision detected
				return true;
			}
		}
	}
	return false;
}

Dot::Dot(int x, int y, std::vector<LTexture> *particleTextures)
{
	// Init the offsets
	mPosX = x;
	mPosY = y;

	// Create pixel collision boxes
	mPixelColliders.resize(11);

	// Set collision box dimensions
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;

	// Init the velocities
	mVelX = 0;
	mVelY = 0;

	// Init pixel collision boxes
	mPixelColliders[0].w = 6;
	mPixelColliders[0].h = 1;
	mPixelColliders[1].w = 10;
	mPixelColliders[1].h = 1;
	mPixelColliders[2].w = 14;
	mPixelColliders[2].h = 1;
	mPixelColliders[3].w = 16;
	mPixelColliders[3].h = 2;
	mPixelColliders[4].w = 18;
	mPixelColliders[4].h = 2;
	mPixelColliders[5].w = 20;
	mPixelColliders[5].h = 6;
	mPixelColliders[6].w = 18;
	mPixelColliders[6].h = 2;
	mPixelColliders[7].w = 16;
	mPixelColliders[7].h = 2;
	mPixelColliders[8].w = 14;
	mPixelColliders[8].h = 1;
	mPixelColliders[9].w = 10;
	mPixelColliders[9].h = 1;
	mPixelColliders[10].w = 6;
	mPixelColliders[10].h = 1;

	// Init colliders relative to position
	shiftColliders();

	// Init Dot particles
	for (int i = 0; i < TOTAL_PARTICLES; i++) {
		particles.push_back(new Particle(mPosX, mPosY, particleTextures));
 	}
}

Dot::~Dot() {
	// Delete particles
	for (int i = 0; i < TOTAL_PARTICLES; ++i) {
		delete particles[i];
	}
}

void Dot::handleEvent(SDL_Event &e, bool p1)
{
	if (p1) { // arrow keys for player 1
		// If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			// Adjust the velocity
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				mVelY -= DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY += DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX -= DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX += DOT_VEL;
				break;
			}
		}
		// If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
			// Adjust the velocity
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				mVelY += DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY -= DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX += DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX -= DOT_VEL;
				break;
			}
		}
	}
	else { // WASD for player 2
		// If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			// Adjust the velocity
			switch (e.key.keysym.sym) {
			case SDLK_w:
				mVelY -= DOT_VEL;
				break;
			case SDLK_s:
				mVelY += DOT_VEL;
				break;
			case SDLK_a:
				mVelX -= DOT_VEL;
				break;
			case SDLK_d:
				mVelX += DOT_VEL;
				break;
			}
		}
		// If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
			// Adjust the velocity
			switch (e.key.keysym.sym) {
			case SDLK_w:
				mVelY += DOT_VEL;
				break;
			case SDLK_s:
				mVelY -= DOT_VEL;
				break;
			case SDLK_a:
				mVelX += DOT_VEL;
				break;
			case SDLK_d:
				mVelX -= DOT_VEL;
				break;
			}
		}
	}
}

void Dot::move(int xlim, int ylim, std::vector<SDL_Rect> *walls, std::vector<Circle> *circles,
	std::vector<SDL_Rect> *pixelColliders, std::vector<Tile *> *TileColliders)
{
	// Move the dot left or right
	mPosX += mVelX;
	mCollider.x = mPosX;
	shiftColliders();

	// If the dot went too far to the left or right
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > xlim)) {
		// Move back
		mPosX -= mVelX;
		mCollider.x = mPosX;
		shiftColliders();
	}
	// Check if it hit a wall
	if (walls != NULL) {
		for (size_t i = 0; i < walls->size(); i++) {
			if (checkCollision(mCollider, (*walls)[i])) {
				// Move back
				mPosX -= mVelX;
				mCollider.x = mPosX;
				shiftColliders();
			}
		}
	}
	// Check if it hit an other dot
	if (pixelColliders != NULL) {
		if (checkPixelCollision(mPixelColliders, *pixelColliders)) {
			// Move back
			mPosX -= mVelX;
			mCollider.x = mPosX;
			shiftColliders();
		}
	}
	// Check if dot hit a circle
	if (circles != NULL) {
		for (size_t i = 0; i < circles->size(); i++) {
			if (checkCollision((*circles)[i], mCollider)) {
				mPosX -= mVelX;
				mCollider.x = mPosX;
				shiftColliders();
			}
		}
	}
	// Check if dot hit tile or victory tile
	if (TileColliders != NULL) {
		if (touchesVictory(mCollider, TileColliders, 1)) {
			mWin = true;
		}
		if (touchesWalls(mCollider, TileColliders, 3,3)) {
			mPosX -= mVelX;
			mCollider.x = mPosX;
			shiftColliders();
		}	
	}

	// Move the dot up or down
	mPosY += mVelY;
	mCollider.y = mPosY;
	shiftColliders();

	// If the dot went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > ylim)) {
		// Move back
		mPosY -= mVelY;
		mCollider.y = mPosY;
		shiftColliders();
	}
	// Check if dot hit wall
	if (walls != NULL) {
		for (size_t i = 0; i < walls->size(); i++) {
			if (checkCollision(mCollider, (*walls)[i])) {
				// Move back
				mPosY -= mVelY;
				mCollider.y = mPosY;
				shiftColliders();
			}
		}
	}
	// Check if dot hit dot
	if (pixelColliders != NULL) {
		if (checkPixelCollision(mPixelColliders, *pixelColliders)) {
			// Move back
			mPosY -= mVelY;
			mCollider.y = mPosY;
			shiftColliders();
		}
	}
	// Check if dot hit circle
	if (circles != NULL) {
		for (size_t i = 0; i < circles->size(); i++) {
			if (checkCollision((*circles)[i], mCollider)) {
				mPosY -= mVelY;
				mCollider.y = mPosY;
				shiftColliders();
			}
		}
	}
	// Check if dot hit tile or victory tile
	if (TileColliders != NULL) {
		if (touchesVictory(mCollider, TileColliders, 1)) {
			mWin = true;
		}
		if (touchesWalls(mCollider, TileColliders, 3, 3)) {
			mPosY -= mVelY;
			mCollider.y = mPosY;
			shiftColliders();
		}
	}
}

std::vector<SDL_Rect> &Dot::getColliders()
{
	return mPixelColliders;
}

void Dot::render(int camX, int camY, LTexture *dotTexture, 
					std::vector<LTexture> *particleTextures, LTexture *ShimmerTexture, 
					LWindow *targetWindow)
{
	dotTexture->render(mPosX - camX, mPosY - camY, targetWindow->getRenderer());

	// Render particles
	renderParticles(targetWindow->getRenderer(), particleTextures, ShimmerTexture);
}

void Dot::shiftColliders()
{
	// The row offset
	int r = 0;

	// Go through the dot's collision boxes
	for (size_t set = 0; set < mPixelColliders.size(); ++set) {
		// Center the collision box
		mPixelColliders[set].x = mPosX + (DOT_WIDTH - mPixelColliders[set].w) / 2;

		// Set the collision box at its row offset
		mPixelColliders[set].y = mPosY + r;

		// Move the row offset down the height of the collision box
		r += mPixelColliders[set].h;
	}
}

Circle *Dot::getCircleCollider()
{
	return mCircleCollider;
}

int Dot::getPosX()
{
	return mPosX;
}

int Dot::getPosY()
{
	return mPosY;
}

void Dot::setPosition(int x, int y)
{
	mPosX = x; mPosY = y;
}

bool Dot::isWin()
{
	return mWin;
}

void Dot::setWin(int status)
{
	mWin = status;
}

void Dot::renderParticles(SDL_Renderer *gRenderer, std::vector<LTexture> *particleTextures,
							LTexture *ShimmerTexture)
{
	// Go through particles
	for (int i = 0; i < TOTAL_PARTICLES; ++i) {
		// Delete and replace dead particles
		if (particles[i]->isDead()) {
			delete particles[i];
			particles[i] = new Particle(mPosX, mPosY, particleTextures);
		}
	}

	// Show particles
	for (int i = 0; i < TOTAL_PARTICLES; ++i) particles[i]->render(gRenderer, ShimmerTexture);
}