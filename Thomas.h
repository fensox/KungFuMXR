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

	// Player control is handled by playerInput() but this checks for other influnces on the player. Namely velocities, gravity, attacks that needs to be factored in to position.
	void move();

private:
	// Walk speed of player
	//Uint32 mWalkWaitTime{ 50 };
	Uint32 mWalkWaitTime{ 10 };

	// Walk distancec of player
	int mWalkDistance{ 20 };

	// Last time we took a step in SDL ticks. Used to regulate walking speed.
	Uint32 mLastWalkTime{};

	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	// Check if enough time has passed to walk then work with viewport position and move player if so
	void attemptWalk(int amount);

	// Adjust the player position or the viewport position. This is called at end of a chain of function calls, begun at playerInput(), that have already confirmed
	// enough time has passed to move, selected the appropriate animation frame, and now we are ready to adjust position of player or viewport depending
	// on where the player is located within the viewport and position of the viewport within the level. The parameter is how much we are attempting to move on each axis.
	void changePosition(SDL_Point amount);

	// Helper function for changePosition function to split some of the code into more manageable pieces. Handles moving left case. Parameters are how far to move and how
	// far we currently are from the center point.
	void changePositionLeft(int movX, int farX);

	// Helper function for changePosition function to split some of the code into more manageable pieces. Handles moving right case. Parameters are how far to move and how
	// far we currently are from the center point.
	void changePositionRight(int movX, int farX);
};

