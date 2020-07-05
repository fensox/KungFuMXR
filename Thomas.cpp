#include "Thomas.h"
#include "SDLMan.h"
#include <memory>
#include <iostream>
#include "Globals.h"

Thomas::Thomas(std::shared_ptr<SDLMan> sdlMan) : Sprite(sdlMan) {
	// set our Thomas specific members
	mMetaFilename = "data/thomas.dat";
	mSpriteSheet = "data/spritesheet.png";
    mActionMode = "WALK_LEFT";
	mTrans = SDL_Color{ 255, 0, 255, 0 };
	mName = "Thomas";
    mScale = 8;
};

// Custom sprite move function
void Thomas::playerInput(SDL_Keycode key) {
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
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (mActionMode != "WALK_RIGHT") {
        mActionMode = "WALK_RIGHT";
        mCurrentFrame = 0;
    } else {
        // only move position on Thomas' stepping frame not standing frame for better looking animation
        if (mCurrentFrame == 0) {
            attemptWalk(mWalkDistance);
        } else {
            attemptWalk(0);
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
        if (mCurrentFrame == 0) {
            attemptWalk(-mWalkDistance);
        } else {
            attemptWalk(0);
        }
    }
}

// Check if enough time has passed to walk and move player given amount if so
void Thomas::attemptWalk(int amount) {
    // we are walking - check if enough time has passed before taking a step
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastWalkTime >= mWalkWaitTime) {
            advanceFrame();
            mPosition.x += amount;
            mLastWalkTime = currentTime;

            //***DEBUG***
            if (FuGlobals::DEBUG_MODE) std::cout << "mActionMode: " << mActionMode << ", Frame: " << mCurrentFrame << std::endl;
    }
}

// Player control is handled by playerInput() but this checks for other influnces on the player. Namely velocity that needs to be factored in.
void Thomas::move() {
    // handle gravity
    
}