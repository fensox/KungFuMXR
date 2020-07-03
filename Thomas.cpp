#include "Thomas.h"
#include "SDLMan.h"
#include <memory>
#include <iostream>

Thomas::Thomas(std::shared_ptr<SDLMan> sdlMan) : Sprite(sdlMan) {
	// set our Thomas specific members
	mMetaFilename = "data/thomas.dat";
	mSpriteSheet = "data/thomas.png";
	mTrans = SDL_Color{ 255, 0, 255, 0 };
	mName = "Thomas";
};

// Custom sprite move function
void Thomas::move(SDL_Keycode key) {
    switch (key)
    {
    case SDLK_UP:

        break;

    case SDLK_DOWN:

        break;

    case SDLK_LEFT:
        moveLeft();
        break;

    case SDLK_RIGHT:
        moveRight();
        break;

    default:

        break;
    }
}

// Handles the player requesting to move to the right.
void Thomas::moveRight() {
    mActionMode = "WALK_RIGHT";
    advanceFrame();
}

// Handles the player requesting to move to the left.
void Thomas::moveLeft() {
    mActionMode = "WALK_LEFT";
    advanceFrame();
}

// Advances the current animation frame ahead or loops to beginning if at end of animation.
void Thomas::advanceFrame() {
    // get the number of frames this animation has
    int count = mAnimMap[mActionMode].size();
    
    // increment the animation ahead or loop back to beginning if we reach the end of the frames available
    if (++mCurrentFrame >= count) mCurrentFrame = 0;
}