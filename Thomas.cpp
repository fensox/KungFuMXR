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
        SDL_Point pnt{ mLevel->getPosition() };
        std::cout << "Viewport: " << pnt.x << ", " << pnt.y << std::endl;
        std::cout << toString() << std::endl;
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
            if (mCurrentFrame == 0) {
                advanceFrame();
                changePosition(mWalkDistance, 0);
                centerViewport();
            } else {
                advanceFrame();
            }
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
                centerViewport();
            } else {
                advanceFrame();
            }
        }
    }
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
    int tmpX{ getX() + moveX };
    int rightBound{ mLevel->getSize().x - FuGlobals::LEVEL_BOUNDS };
    if (tmpX < FuGlobals::LEVEL_BOUNDS) tmpX = FuGlobals::LEVEL_BOUNDS;
    else if (tmpX > rightBound) tmpX = rightBound;

    // Commit the move to the player position
    setX(tmpX);
}

// Player control is handled by playerInput() but this checks for other influnces on the player. Namely velocities, gravity, attacks that needs to be factored in to position.
void Thomas::move() {
    // handle gravity
    
}

// Calls the level's centerViewport() function passing in new player coordinates to center around. Called after a player movement.
void Thomas::centerViewport() {
    mLevel->centerViewport(getX(), getY());
}