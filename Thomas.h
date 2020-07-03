#pragma once
#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class Thomas : public Sprite
{
public:
	// Use base Sprite constructor
	Thomas(std::shared_ptr<SDLMan> sdlMan);

	// custom sprite move function
	void move(SDL_Keycode key);

private:
	// Walk speed of player
	Uint32 mWalkWaitTime{ 70 };

	// Walk distancec of player
	int mWalkDistance{ 16 };

	// Last time we took a step in SDL ticks. Used to regulate walking speed.
	Uint32 mLastWalkTime{};

	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	// Advances the current animation frame ahead or loops to beginning if at end of animation.
	void advanceFrame();

	// Check if enough time has passed to walk and move player given amount if so
	void attemptWalk(int amount);
};

