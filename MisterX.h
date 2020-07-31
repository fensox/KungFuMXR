#pragma once
#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class MisterX : public Sprite
{
public:
	const decimal	WALK_VELOCITY_PER		{ 100 };		// Walk velocity increase per real world second. Higher than WALK_MAX to overcome global friction constants.
	const decimal	WALK_MAX				{ 7 };			// Maximum velocity player can walk per real world second
	const Uint32	WALK_WAIT_TIME			{ 250 };		// Milliseconds between change of animation 
	const decimal	JUMP_VELOCITY			{ 8.5 };		// Initial force a sprite generates to start a jump in pixels per second
	const Uint32	ATTACK_HOLD				{ 1000 };		// Milliseconds to hold an attack animation on screen before returning to former animation

	// Use base Sprite constructor
	MisterX(std::shared_ptr<SDLMan> sdlMan);

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
	// If we are actively walking left. Used for all animations, as well as walking, to indicate direction.
	bool mWalkingLeft{ false };

	// If we are actively walking right. Used for all animations, as well as walking, to indicate direction.
	bool mWalkingRight{ false };

	// If we are actively jumping
	bool mJumping{ false };

	// If we are actively ducking
	bool mDucking{ false };

	// if we are actively punching
	bool mPunching{ false };

	// Holds the SDL ticks the last time an animation frame changed
	Uint32 mLastAnimStep{};

	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	// Handles the player initiating a jump.
	void jump();

	// Handles the player initiating a duck.
	void duck();

	// Handles the player initiating a punch
	void punch();

	// Adjust the player position back inside the level if an out of bounds location has been detected.
	void adjustForLevelBounds();

	// Return bool whethar enough time has passed to change walk animation.
	bool checkWalkTime();

	//***DEBUG*** Outputs some debugging info
	void outputDebug();
};

