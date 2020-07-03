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
    mScale = 4;

    // set our position
    int startWidth{ mAnimMap[mActionMode].at(0).w };
    int startHeight{ mAnimMap[mActionMode].at(0).h };
    mPosition.x = sdlMan->getWindowW() - (startWidth * mScale);
    mPosition.y = sdlMan->getWindowH() - (startHeight * mScale);
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
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (mActionMode != "WALK_RIGHT") {
        mActionMode = "WALK_RIGHT";
        mCurrentFrame = 0;
    } else {
        //attempts to move the player the given amount if enough time has passed
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
    }
}

// Advances the current animation frame ahead or loops to beginning if at end of animation.
void Thomas::advanceFrame() {
    // get the number of frames this animation has
    std::size_t count = mAnimMap[mActionMode].size();

    // increment the animation ahead or loop back to beginning if we reach the end of the frames available
    if (++mCurrentFrame >= count) mCurrentFrame = 0;
}