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
	bool checkWalkTime();

	// Adjust the player position or the viewport position. This is called at end of a chain of function calls, begun at playerInput(), that have already confirmed
	// enough time has passed to move, selected the appropriate animation frame, and now we are ready to adjust position
	void changePosition(int moveX, int moveY);

	// Calls the level's centerViewport() function passing in new player coordinates to center around. Called after a player movement.
	void centerViewport();
};

