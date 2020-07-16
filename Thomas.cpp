#include "Thomas.h"
#include "SDLMan.h"
#include "FensoxUtils.h"
#include <memory>
#include <iostream>
#include <cstdlib>
#include "Globals.h"

Thomas::Thomas(std::shared_ptr<SDLMan> sdlMan) : Sprite(sdlMan) {
	// set our Thomas specific members
	mMetaFilename = "data/thomas.dat";
	mSpriteSheet = "data/spritesheet.png";
    mActionMode = "WALK_LEFT";
	mTrans = SDL_Color{ 255, 0, 255, 0 };
	mName = "Thomas";
    mScale = 4;
};

// Handles input from the player. SDL_Keycode is the key and the bool is true on key pressed and false on key released.
void Thomas::playerInput(SDL_Keycode key, bool press) {
    switch (key) {
        case SDLK_UP:
            std::cout << mXPos << ", " << mYPos << std::endl;
            break;

        case SDLK_SPACE:
            JUMP = press;
            break;

        case SDLK_DOWN:

            break;

        case SDLK_LEFT:
            LEFT = press;
            break;

        case SDLK_RIGHT:
            RIGHT = press;
            break;

        default:

            break;
    }
}

// Handles the player requesting to move to the right.
void Thomas::moveRight() {
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (mActionMode != "WALK_RIGHT") {
        mActionMode = "WALK_RIGHT";
        mCurrentFrame = 0;
    } else {
        //attempts to move the player the given amount if enough time has passed
        if (checkWalkTime()) {
            advanceFrame();
            mVeloc.right += WALK_VELOCITY_PER;
            if (mVeloc.right > WALK_MAX) mVeloc.right = WALK_MAX;
        }
    }
}

// Handles the player requesting to move to the left.
void Thomas::moveLeft() {
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (mActionMode != "WALK_LEFT") {
        mActionMode = "WALK_LEFT";
        mCurrentFrame = 0;
    } else {
        // increase velocity if enough time has passed
        if (checkWalkTime()) {
            advanceFrame();
            mVeloc.left += WALK_VELOCITY_PER;
            if (mVeloc.left > WALK_MAX) mVeloc.left = WALK_MAX;
        }
    }
}

// Handles the player requesting a jump.
void Thomas::jump() {
    // only launch into a jump if we have something to launch off of
    if (downBump() && mVeloc.up <= 0) {
        mVeloc.up += JUMP_VELOCITY;
    }
}

// Check if enough time has passed to walk and if so return true otherwise false.
bool Thomas::checkWalkTime() {
    // we are walking - check if enough time has passed before taking a step
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastWalkTime >= WALK_WAIT_TIME) {
        mLastWalkTime = currentTime;
        return true;
    } else {
        return false;
    }
}

// Adjust the player position back inside the level if an out of bounds location has been detected.
void Thomas::adjustForLevelBounds() {
    int downBound{ mLevel->getSize().y - FuGlobals::LEVEL_BOUNDS };
    int rightBound{ mLevel->getSize().x - FuGlobals::LEVEL_BOUNDS };
    
    // x
    if (mXPos < FuGlobals::LEVEL_BOUNDS) mXPos = FuGlobals::LEVEL_BOUNDS;
    else if (mXPos > rightBound) mXPos = rightBound;

    // y
    if (mYPos < FuGlobals::LEVEL_BOUNDS) mYPos = FuGlobals::LEVEL_BOUNDS;
    else if (mYPos > downBound) mYPos = downBound;
}

// Moves player based on velocities adjusting for gravity, friction, and collisions. Overrides the Sprite class default move function
// for a few custom player effects like respecting level boundries that other sprites do not need to do.
void Thomas::move() {
    if (JUMP) jump();
    if (LEFT) moveLeft();
    if (RIGHT) moveRight();

    Sprite::move();

    adjustForLevelBounds();
}