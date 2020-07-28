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
    mScale = 3;
};

//***DEBUG*** Outputs some debugging info
void Thomas::outputDebug() {
    std::cout << "Player:\t\t\t" << mXPos << "\t" << mYPos << "\t" << getRect().w * mScale << "\t" << getRect().h * mScale << "\n";
    std::cout << "P Veloc L, R / U, D:\t" << mVeloc.left << "\t" << mVeloc.right << "\t/\t" << mVeloc.up << "\t" << mVeloc.down << "\n";
    std::cout << "Collision Rect:\t\t" << getCollisionRect().x << "\t" << getCollisionRect().y << "\t" << getCollisionRect().w << "\t" << getCollisionRect().h << "\n";
    std::cout << "Downbump:\t\t" << std::boolalpha << downBump() << std::endl;
}

// Handles jopystick input from the player.
void Thomas::playerInputPadStick(const SDL_ControllerAxisEvent e) {
    using namespace FuGlobals;

    //X axis motion
    if (e.axis == 0) {
        if (e.value < -JOYSTICK_DEAD_ZONE) {          // Left of dead zone
            mWalkingLeft = true;
            mWalkingRight = false;
        } else if (e.value > JOYSTICK_DEAD_ZONE) {    // Right of dead zone
            mWalkingRight = true;
            mWalkingLeft = false;
        } else {                                      // Stick not engaged on x axis
            mWalkingRight = false;
            mWalkingLeft = false;
        }
    } else if (e.axis == 1) {                         // Y axis motion
        if (e.value < -JOYSTICK_DEAD_ZONE) {          // Up above dead zone
            // Analog stick up
        } else if (e.value > JOYSTICK_DEAD_ZONE) {    // Down below dead zone
            // Analog stick down
        } else {                                      // Stick not engaged on y axis
            // Analog stick not up or down
        }
    }
}

// Handles gamepad button input from the player.
void Thomas::playerInputPadBtn(const SDL_ControllerButtonEvent e, bool press) {
    switch (e.button) {
        case SDL_CONTROLLER_BUTTON_A:
            mJumping = press;
            break;
        case SDL_CONTROLLER_BUTTON_B:
            // B button
            break;
        case SDL_CONTROLLER_BUTTON_X:
            // X button
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            // Y button
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            // dpad up
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            // dpad down
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            mWalkingLeft = press;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            mWalkingRight = press;
            break;
        case SDL_CONTROLLER_BUTTON_START:
            // start button
            break;
        case SDL_CONTROLLER_BUTTON_BACK:
            // back button (or select on NES type controller)
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            // left shoulder button
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            // right shoulder button
            break;

    }
}

// Handles keyboard input from the player. SDL_Keycode is the key and the bool is true on key pressed and false on key released.
void Thomas::playerInput(const SDL_Keycode& key, bool press) {
    switch (key) {
        case SDLK_UP:
            //***DEBUG***
            outputDebug();
            break;

        case SDLK_SPACE:
            mJumping = press;
            break;

        case SDLK_DOWN:

            break;

        case SDLK_LEFT:
            mWalkingLeft = press;
            break;

        case SDLK_RIGHT:
            mWalkingRight = press;
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
        // increase velocity if enough time has passed
        if (checkWalkTime()) {
            advanceFrame();
            mVeloc.right += WALK_VELOCITY_PER;
            if (mVeloc.right > WALK_MAX) mVeloc.right = WALK_MAX;
            //***DEBUG***
            std::cout << mVeloc.right << std::endl;
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
            //***DEBUG***
            std::cout << mVeloc.left << std::endl;
        }
    }
}

// Handles the player requesting a jump.
void Thomas::jump() {
    // check if enough time has passed before another jump is allowed
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastJumpTime >= JUMP_WAIT_TIME) {
        // only launch into a jump if we have something to launch off of
        if (downBump() && mVeloc.up <= 0) {
            mVeloc.up += JUMP_VELOCITY;
            outputDebug();
            mLastJumpTime = currentTime;
        }
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
    int downBound{ mLevel.lock()->getSize().y - FuGlobals::LEVEL_BOUNDS };
    int rightBound{ mLevel.lock()->getSize().x - FuGlobals::LEVEL_BOUNDS };
    
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
    if (mJumping) jump();
    if (mWalkingLeft) moveLeft();
    if (mWalkingRight) moveRight();

    Sprite::move();

    adjustForLevelBounds();
}