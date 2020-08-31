#pragma once

#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class StickMan : public Sprite {

public:
	static constexpr decimal	WALK_VELOCITY_PER	{ 75 };			// Walk velocity increase per real world second. Higher than WALK_MAX to overcome global friction constants.
	static constexpr decimal	WALK_MAX			{ 2.0 };		// Maximum velocity can walk per real world second
	static constexpr Uint32		WALK_WAIT_TIME		{ 250 };		// Milliseconds between change of animation 

	StickMan(std::weak_ptr<SDLMan> mSDL);

	// Extend Sprite's move() function for some AI then call Sprite's function for movement based on velocity, gravity, and collision detection, etc.
	void move();

private:
	// Holds the SDL ticks the last time an animation frame changed for the walking action
	Uint32 mLastAnimStep{};

	// Move to the right
	void moveRight();

	// Move to the left
	void moveLeft();

	// Check if enough time has passed so we can advance walking animation frame
	bool checkWalkTime();
};
