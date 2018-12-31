#pragma once

#include <stdio.h>
#include <iostream>
#include <SDL.h>

class LTimer
{
public:
	LTimer();
	~LTimer();

	// The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	// Get's the timer's time
	Uint32 getTicks();

	// Check the status of the timer
	bool isStarted();
	bool isPaused();

private:
	// The clock time when the timer started
	Uint32 mStartTicks;

	// The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	// The timer status
	bool mPaused;
	bool mStarted;
};

