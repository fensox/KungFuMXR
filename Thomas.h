#pragma once
#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class Thomas : public Sprite
{
public:
	// Use base Sprite constructor
	Thomas(std::shared_ptr<SDLMan> sdlMan);

	// Handles input from the player and performs sprite actions accordingly
	void playerInput(SDL_Keycode key);

	// Implementation of Sprite class virtual function. Handle movement of Thomas not initiated by the player
	// i.e. outside forces acting on player.
	void move();

private:
	// Walk speed of player
	Uint32 mWalkWaitTime{ 50 };

	// Walk distancec of player
	//int mWalkDistance{ 20 };
	int mWalkDistance{ 35 };

	// Last time we took a step in SDL ticks. Used to regulate walking speed.
	Uint32 mLastWalkTime{};

	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	// Check if enough time has passed to walk and move player given amount if so
	void attemptWalk(int amount);
};

