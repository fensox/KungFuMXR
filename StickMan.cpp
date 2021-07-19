#include "StickMan.h"
#include <iostream>

// constructor
StickMan::StickMan(std::weak_ptr<SDLMan> mSDL) : Sprite{ mSDL } {
	// set our Stick Man specific data
	mMetaFilename = "data/StickMan.dat";
	mSpriteSheet = "data/MasterSS.png";
	mStartingActionMode = "WALK_RIGHT";
	mTrans = SDL_Color{ 255, 0, 255, 0 };
	mName = "StickMan";
	mScale = 3;
    setHealth(100);

	// set default action mode set for this sprite into our action mode member
	setActionMode( mStartingActionMode, true );

	// load sound effects
	//mSDL.lock()->addSoundEffect("MRX_PUNCH", "data/mrx_punch.wav");
	//mSDL.lock()->addSoundEffect("MRX_KICK", "data/mrx_kick.wav");

}

// Check if enough time has passed so we can advance walking animation frame
bool StickMan::checkWalkTime() {
    // we are walking - check if enough time has passed before taking a step
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastAnimStep >= WALK_WAIT_TIME) {
        mLastAnimStep = currentTime;
        return true;
    } else {
        return false;
    }
}

// Move to the right
void StickMan::moveRight() {
    using namespace FuGlobals;

    // if the previous action was different set new mActionMode, set animation frame to 0, and don't move position this frame
    if (getActionMode() != "WALK_RIGHT") {
        mFacingRight = true;
        setActionMode("WALK_RIGHT", true);
    } else {
        // step animation frame if enough time has passed and we're not pressed up against an object
        int frameWidth{ (getCollisionRect().w / 2) + 1 };
        if (checkWalkTime()
            && !isCollision(ColType::CT_LEVEL, ColDirect::CD_RIGHT, frameWidth)
            && !isCollision(ColType::CT_SPRITE, ColDirect::CD_RIGHT, frameWidth))
        {
            advanceFrame();
        }

        // increase velocity based on FPS calc to reach our per second goal
        mVeloc.right += WALK_VELOCITY_PER / mSDL.lock()->getFPS();
        if (mVeloc.right > WALK_MAX) mVeloc.right = WALK_MAX;
    }
}

// Move to the left
void StickMan::moveLeft() {
    using namespace FuGlobals;

    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move position this frame
    if (getActionMode() != "WALK_LEFT") {
        mFacingRight = false;
        setActionMode("WALK_LEFT", true);
    } else {
        // step animation frame if enough time has passed and we're not pressed up against an object
        int frameWidth{ (getCollisionRect().w / 2) + 1 };
        if (checkWalkTime()
            && !isCollision(ColType::CT_LEVEL, ColDirect::CD_LEFT, frameWidth)
            && !isCollision(ColType::CT_SPRITE, ColDirect::CD_LEFT, frameWidth))
        {
            advanceFrame();
        }

        // increase velocity based on FPS calc to reach our per second goal
        mVeloc.left += WALK_VELOCITY_PER / mSDL.lock()->getFPS();
        if (mVeloc.left > WALK_MAX) mVeloc.left = WALK_MAX;
    }
}

// Extend Sprite's move() function for some AI then call Sprite's function for movement based on velocity, gravity, and collision detection, etc.
void StickMan::move() {
    // Walk towards the player if we are not colliding with them
    if (mTargetSprite.lock()->getX() > getX()) {
        moveRight();
    } else if (mTargetSprite.lock()->getX() < getX()) {
        moveLeft();
    }

    // call parent function for gravity, friction, & collision detection
    Sprite::move();
}