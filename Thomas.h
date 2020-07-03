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
	// Handles the player requesting to move to the right.
	void moveRight();

	// Handles the player requesting to move to the right.
	void moveLeft();

	// Advances the current animation frame ahead or loops to beginning if at end of animation.
	void advanceFrame();
};

