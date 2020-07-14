#include "Thomas.h"
#include "SDLMan.h"
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

// Custom sprite move function
void Thomas::playerInput(SDL_Keycode key) {
    switch (key)
    {
    case SDLK_UP:
        //***DEBUG***
        if (FuGlobals::DEBUG_MODE) std::cout << "Velocity (Up, Down, Left, Right): " << mVeloc.up << ", " << mVeloc.down << ", " << mVeloc.left << ", " << mVeloc.right << std::endl;
        break;

    case SDLK_SPACE:
        jump();
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
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (mActionMode != "WALK_RIGHT") {
        mActionMode = "WALK_RIGHT";
        mCurrentFrame = 0;
    } else {
        //attempts to move the player the given amount if enough time has passed
        if (checkWalkTime()) {
            advanceFrame();
            if (mCurrentFrame == 0) changePosition(mWalkDistance, 0);
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
        //attempts to move the player the given amount if enough time has passed
        if (checkWalkTime()) {
            if (mCurrentFrame == 0) {
                advanceFrame();
                changePosition(-mWalkDistance, 0);
            } else {
                advanceFrame();
            }
        }
    }
}

// Handles the player requesting a jump.
void Thomas::jump() {
    // only launch into a jump if we have something to launch off of
    if (downBump()) {
        mVeloc.up += mJumpPower;
    }

    //***DEBUG***
    if (FuGlobals::DEBUG_MODE) std::cout << "Velocity (Up, Down, Left, Right): " << mVeloc.up << ", " << mVeloc.down << ", " << mVeloc.left << ", " << mVeloc.right << std::endl;
}

// Check if enough time has passed to walk and if so return true otherwise false.
bool Thomas::checkWalkTime() {
    // we are walking - check if enough time has passed before taking a step
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastWalkTime >= mWalkWaitTime) {
        mLastWalkTime = currentTime;
        return true;
    } else {
        return false;
    }
}

// Adjust the player position. This is called at end of a chain of function calls, begun at playerInput(), that have already confirmed
// enough time has passed to move, selected the appropriate animation frame, and now we are ready to adjust position of player.
void Thomas::changePosition(int moveX, int moveY) {
    // Checks for level boundries being exceeded and adjusts if necessary
    int tmpX{ mXPos + moveX };
    int rightBound{ mLevel->getSize().x - FuGlobals::LEVEL_BOUNDS };
    if (tmpX < FuGlobals::LEVEL_BOUNDS) tmpX = FuGlobals::LEVEL_BOUNDS;
    else if (tmpX > rightBound) tmpX = rightBound;

    // Commit the move to the player position
    mXPos = tmpX;
}