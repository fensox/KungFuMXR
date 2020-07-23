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

	// Handles jopystick input from the player.
	void playerInputJoystick(const SDL_Event& e);

	// Moves player based on velocities adjusting for gravity, friction, and collisions. Overrides the Sprite class default move function
	// for a few custom player effects like respecting level boundries that other sprites do not need to do.
	void move();

private:
	// Time between walk speed increases
	const Uint32 WALK_WAIT_TIME { 100 };

	// Walk velocity increase per mWalkWaitTime
	const decimal WALK_VELOCITY_PER { 5 };

	// Maximum velocity player can walk
	const decimal WALK_MAX { 5 };

	// Jump power is the amount added to upward velocity when jump action initiated
	const decimal JUMP_VELOCITY { 6 };

	// Time after a jump that has to pass before another jump
	const Uint32 JUMP_WAIT_TIME{ 0 };

	// Last time we took a step in SDL ticks. Used to regulate walking speed.
	Uint32 mLastWalkTime{};

	// Last time we jumped in SDL ticks. Used to regulate time between jumps.
	Uint32 mLastJumpTime{};

	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	//***DEBUG***
	// Control switches
	bool RIGHT{ false }, LEFT{ false }, JUMP{ false };

	// Handles the player requesting a jump.
	void jump();

	// Check if enough time has passed to walk then work with viewport position and move player if so
	bool checkWalkTime();

	// Adjust the player position back inside the level if an out of bounds location has been detected.
	void adjustForLevelBounds();

	//***DEBUG*** Outputs some debugging info
	void outputDebug();
};

