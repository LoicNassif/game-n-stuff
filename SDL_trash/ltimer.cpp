#include "ltimer.h"
#include <stdio.h>


LTimer::LTimer()
{
	// Init the vars
	mStartTicks = 0;
	mPausedTicks = 0;
	
	mPaused = false;
	mStarted = false;
}

LTimer::~LTimer() {}

void LTimer::start()
{
	// Start the timer
	mStarted = true;

	// Unpaused the timer
	mPaused = false;

	// Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop() 
{
	// Stop the timer
	mStarted = false;

	// Unpause the timer
	mPaused = false;

	// Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause() 
{
	// If the timer is running and isn't already paused
	if (mStarted && !mPaused) {
		// Pause the timer
		mPaused = true;

		// Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause() 
{
	// If the timer is running and paused
	if (mStarted && mPaused) {
		// Unpause the timer
		mPaused = false;

		// Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		// Reset the paused ticks
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks() 
{
	// If the timer is running
	if (mStarted) {
		// If the timer is paused
		if (mPaused) {
			// Return the number of ticks when the timer was paused
			return mPausedTicks;
		}
		else {
			// Return the current time minus the start tiem
			return (SDL_GetTicks() - mStartTicks);
		}
	}
	else {
		// Return a stopped timer
		return 0;
	}
}

bool LTimer::isStarted()
{
	return mStarted;
}

bool LTimer::isPaused() 
{
	return mPaused && mStarted;
}

