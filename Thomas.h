#pragma once
#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class Thomas : public Sprite
{
public:
	// Use base Sprite constructor
	Thomas(std::shared_ptr<SDLMan> sdlMan);

	// Handles keyboard input from the player. SDL_Keycode is the key and the bool is true on key pressed and false on key released.
	void playerInput(const SDL_Keycode& key, bool press);

	// Handles gamepad stick input from the player.
	void playerInputPadStick(const SDL_ControllerAxisEvent e);

	// Handles gamepad button input from the player. bool press is true on key pressed and false on key released.
	void playerInputPadBtn(const SDL_ControllerButtonEvent e, bool press);

	// Moves player based on velocities adjusting for gravity, friction, and collisions. Overrides the Sprite class default move function
	// for a few custom player effects like respecting level boundries that other sprites do not need to do.
	void move();

private:
	// Time between walk speed increases
	const Uint32 WALK_WAIT_TIME { 100 };

	// Walk velocity increase per WALK_WAIT_TIME
	const decimal WALK_VELOCITY_PER { 2 };

	// Maximum velocity player can walk
	const decimal WALK_MAX { 4.0 };

	// Last time we took a step in SDL ticks. Used to regulate walking speed.
	Uint32 mLastWalkTime{};

	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	// Check if enough time has passed to walk then work with viewport position and move player if so
	bool checkWalkTime();

	// Adjust the player position back inside the level if an out of bounds location has been detected.
	void adjustForLevelBounds();

	//***DEBUG*** Outputs some debugging info
	void outputDebug();
};

