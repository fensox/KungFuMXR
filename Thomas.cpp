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
            if (mCurrentFrame == 0) {
                advanceFrame();
                changePosition(WALK_VELOCITY_PER, 0);
            } else {
                advanceFrame();
            }
        }
    }
}


//***DEBUG*** Old moveLeft function -being kept until velocity method of movement perfected
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
                changePosition(-WALK_VELOCITY_PER, 0);
            } else {
                advanceFrame();
            }
        }
    }
}
//*/
/*
// Handles the player requesting to move to the left.
void Thomas::moveLeft() {
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (mActionMode != "WALK_LEFT") {
        mActionMode = "WALK_LEFT";
        mCurrentFrame = 0;
    } else {
        //increase player velocity (to a maximum velocity) if enough time has passed and we are on the correct animation frame
        if (checkWalkTime()) {
            if (mCurrentFrame == 0) {
                advanceFrame();
                //changePosition(-mWalkDistance, 0);
                mVeloc.left += WALK_VELOCITY_PER;
                if (mVeloc.left > WALK_MAX) mVeloc.left = WALK_MAX
            } else {
                advanceFrame();
            }
        }
    }
}
*/
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

// Adjust the player position. This is called at end of a chain of function calls, begun at playerInput(), that have already confirmed
// enough time has passed to move, selected the appropriate animation frame, and now we are ready to adjust position of player.
void Thomas::changePosition(decimal moveX, decimal moveY) {
    // Checks for level boundries being exceeded and adjusts if necessary
    decimal tmpX{ mXPos + moveX };
    int rightBound{ mLevel->getSize().x - FuGlobals::LEVEL_BOUNDS };
    if (tmpX < FuGlobals::LEVEL_BOUNDS) tmpX = FuGlobals::LEVEL_BOUNDS;
    else if (tmpX > rightBound) tmpX = rightBound;

    // Commit the move to the player position
    mXPos = tmpX;
}

// Moves player based on velocities adjusting for gravity, friction, and collisions. Overrides the Sprite class default move function
// for a few custom player effects like respecting level boundries that other sprites do not need to do.
void Thomas::move() {
    // apply gravity - if not downBump'ing increase down velocity by our gravity global every time interval specified by our gravity time interval
    if (downBump()) {
        // we are standing on something, if its the end of a fall, stop and downward velocity AND upward velocity to make sure up/down both canceled out.
        // Stops a bouncing player effect if they have not evenly canceled each other out upon landing.
        if (mVeloc.down != 0) {
            mVeloc.up = 0;
            mVeloc.down = 0;
        }
    } else {
        if (SDL_GetTicks() - mLastGravTime >= FuGlobals::GRAVITY_TIME) {
            mVeloc.up -= FuGlobals::GRAVITY;
            if (mVeloc.up < 0) mVeloc.up = 0;
            mVeloc.down += FuGlobals::GRAVITY;
            mLastGravTime = SDL_GetTicks();
        }
    }

    // temporarily adjust position based on velocities then we test if we can move that much without a collision
    decimal tryX = mXPos + mVeloc.right - mVeloc.left;
    decimal tryY = mYPos - mVeloc.up + mVeloc.down;
    PointF pnt{0.f, 0.f};

    // x test and possible reduction in distance
    int tmpPos = static_cast<int>(tryX);
    if (tryX > 0) {
        for (int i{ tmpPos }; i > 0; --i) {
            pnt = { i, tryY };
            if (!mLevel->isACollision( pnt )) break;
        }
    } else if (tryX < 0) {
        for (int i{ tmpPos }; i < 0; ++i) {
            pnt = { i, tryY };
            if (!mLevel->isACollision( pnt )) break;
        }
    }

    // y test and possible reduction in distance
    tmpPos = static_cast<int>(tryY);
    if (tryY > 0) {
        for (int i{ tmpPos }; i > 0; --i) {
            pnt = { tryX, i };
            if (!mLevel->isACollision( pnt )) break;
        }
    } else if (tryY < 0) {
        for (int i{ tmpPos }; i < 0; ++i) {
            pnt = { tryX, i };
            if (!mLevel->isACollision( pnt )) break;
        }
    }

    // set the position now that all tests have completed
    mXPos = tryX;
    mYPos = tryY;
    std::cout << "Thomas u/d: " << mVeloc.up << ", " << mVeloc.down << "               x/y: " << mXPos << ", " << mYPos << std::endl;
}