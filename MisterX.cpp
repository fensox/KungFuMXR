#include "MisterX.h"
#include "FensoxUtils.h"
#include "Globals.h"
#include <iostream>
#include <cstdlib>

MisterX::MisterX(std::shared_ptr<SDLMan> sdlMan) : Sprite(sdlMan) {
	// set our Mr. X specific members
	mMetaFilename = "data/MisterX.dat";
	mSpriteSheet = "data/MasterSS.png";
    mStartingActionMode = "WALK_LEFT";
	mTrans = SDL_Color{ 255, 0, 255, 0 };
	mName = "MisterX";
    mScale = 3;
};

//***DEBUG*** Outputs some debugging info
void MisterX::outputDebug() {
    std::cout << "Player: " << mXPos << "\t" << mYPos << "\t" << getRect().w * mScale << "\t" << getRect().h * mScale << "\n";
    std::cout << "P Veloc L, R / U, D: " << mVeloc.left << "\t" << mVeloc.right << "\t/\t" << mVeloc.up << "\t" << mVeloc.down << "\n";
    std::cout << "Collision Rect: " << getCollisionRect().x << "\t" << getCollisionRect().y << "\t" << getCollisionRect().w << "\t" << getCollisionRect().h << "\n";
    std::cout << "mActionMode: " << getActionMode() << "\tmLastActionMode: " << getLastActionMode() << "\tmCurrentFrame: " << mCurrentFrame << "\n";
    std::cout << "Downbump:" << std::boolalpha << downBump() << "\tFPS: " << mSDL.lock()->getFPS() << "\n";
}

// Handles jopystick input from the player.
void MisterX::playerInputPadStick(const SDL_ControllerAxisEvent e) {
    using namespace FuGlobals;

    //X axis motion
    if (e.axis == 0) {
        if (e.value < -JOYSTICK_DEAD_ZONE) {                                // Left of dead zone
            mWalkingLeft = true;
            mWalkingRight = false;
        } else if (e.value > JOYSTICK_DEAD_ZONE) {                          // Right of dead zone
            mWalkingRight = true;
            mWalkingLeft = false;
        } else {                                                            // Stick not engaged on x axis
            mWalkingRight = false;
            mWalkingLeft = false;
        }
    } else if (e.axis == 1) {                                               // Y axis motion
        if (e.value < -JOYSTICK_DEAD_ZONE) {                                // Up above dead zone
            mDucking = false;
        } else if (e.value > JOYSTICK_DEAD_ZONE) {                          // Down below dead zone
            mDucking = true;
            // initial call to go into duck anim or come out of it
            duck();
        } else {                                                            // Stick not engaged on y axis
            mDucking = false;
            // initial call to go into duck anim or come out of it
            duck();
        }
    }
}

// Handles gamepad button input from the player.
void MisterX::playerInputPadBtn(const SDL_ControllerButtonEvent e, bool press) {
    switch (e.button) {
        case SDL_CONTROLLER_BUTTON_A:
            mJumping = press;
            break;
        case SDL_CONTROLLER_BUTTON_B:
            // B button
            break;
        case SDL_CONTROLLER_BUTTON_X:
            mPunching = true;
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            // Y button
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            // dpad up
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            if (press) mDucking = true;
            duck(); // initial call to go into duck anim or come out of it
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
void MisterX::playerInput(const SDL_Keycode& key, bool press) {
    switch (key) {
        case SDLK_UP:
            //***DEBUG***
            if (FuGlobals::DEBUG_MODE) outputDebug();
            break;

        case SDLK_SPACE:
            mJumping = press;
            break;

        case SDLK_DOWN:
            mDucking = press;
            duck(); // initial call to go into duck anim or come out of it
            break;

        case SDLK_LEFT:
            mWalkingLeft = press;
            break;

        case SDLK_RIGHT:
            mWalkingRight = press;
            break;

        case SDLK_a:
            if (press && mAttackReleased) {
                mAttacking = true;
                mPunching = true;
                mAttackReleased = false;
            } else if (!press) {
                mAttackReleased = true;
            }
            break;

        default:

            break;
    }
}

// Handles the player requesting to move to the right.
void MisterX::moveRight() {
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (getActionMode() != "WALK_RIGHT") {
        setActionMode("WALK_RIGHT");
        mFacingRight = true;
    } else {
        // step animation frame if enough time has passed
        if (checkWalkTime()) advanceFrame();

        // increase velocity based on FPS calc to reach our per second goal
        mVeloc.right += WALK_VELOCITY_PER / mSDL.lock()->getFPS();
        if (mVeloc.right > WALK_MAX) mVeloc.right = WALK_MAX;
    }
}

// Handles the player requesting to move to the left.
void MisterX::moveLeft() {
    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if (getActionMode() != "WALK_LEFT") {
        setActionMode("WALK_LEFT");
        mFacingRight = false;
    } else {
        // step animation frame if enough time has passed
        if (checkWalkTime()) advanceFrame();

        // increase velocity based on FPS calc to reach our per second goal
        mVeloc.left += WALK_VELOCITY_PER / mSDL.lock()->getFPS();
        if (mVeloc.left > WALK_MAX) mVeloc.left = WALK_MAX;
    }
}

// Return bool whethar enough time has passed to change walk animation. Resets timer on true result.
bool MisterX::checkWalkTime() {
    // we are walking - check if enough time has passed before taking a step
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastAnimStep >= WALK_WAIT_TIME) {
        mLastAnimStep = currentTime;
        return true;
    } else {
        return false;
    }
}

// Adjust the player position back inside the level if an out of bounds location has been detected.
void MisterX::adjustForLevelBounds() {
    int downBound{ mLevel.lock()->getSize().y - FuGlobals::LEVEL_BOUNDS };
    int rightBound{ mLevel.lock()->getSize().x - FuGlobals::LEVEL_BOUNDS };
    
    // x
    if (mXPos < FuGlobals::LEVEL_BOUNDS) mXPos = FuGlobals::LEVEL_BOUNDS;
    else if (mXPos > rightBound) mXPos = rightBound;

    // y
    if (mYPos < FuGlobals::LEVEL_BOUNDS) mYPos = FuGlobals::LEVEL_BOUNDS;
    else if (mYPos > downBound) mYPos = downBound;
}

// Handles player initiating a jump.
void MisterX::jump() {
    // only launch into a jump if we have something to launch off of
    if (downBump()) {
        mVeloc.down = 0;
        mVeloc.up = JUMP_VELOCITY;
    }
}

// Handles the player initiating a duck.
void MisterX::duck() {
    if (mDucking) {
        if (mFacingRight) {
            setActionMode("DUCK_RIGHT");
        } else {
            setActionMode("DUCK_LEFT");
        }
    } else {
        // will restore walking anim frame but won't move player as player doesn't move first time an actionmode changes
        if (mFacingRight) {
            moveRight();
        } else {
            moveLeft();
        }
    }
}

// Handles the player initiating a punch. mPunching bool not tied to button release like other actions. We turn off when animation complete to end punching action.
void MisterX::punch() {
    using namespace FensoxUtils;

    // set our punch start time if this is our first frame of attack
    if (getActionMode().find("PUNCH_") == std::string::npos) mAttackTime = SDL_GetTicks();
    
    // if not in punch mode yet pick correct punch mode
    switch (hash( getActionMode().c_str() )) {
        case (hash("DUCK_RIGHT")):
            setActionMode("PUNCH_DUCK_RIGHT");
            break;
        case (hash("DUCK_LEFT")):
            setActionMode("PUNCH_DUCK_LEFT");
            break;
        case (hash("WALK_RIGHT")):
            setActionMode("PUNCH_RIGHT");
            break;
        case (hash("WALK_LEFT")):
            setActionMode("PUNCH_LEFT");
            break;
    }

    // stay in punch mode animation until ATTACK_TIME has passed
    Uint32 time{ SDL_GetTicks() - mAttackTime };
    if ((time) >= ATTACK_TIME) {
        setActionMode( getLastActionMode() );
        mPunching = false;
        mAttacking = false;
    }
}

// Moves player based on velocities adjusting for gravity, friction, and collisions. Extends then calls the Sprite class
// default move function for a few custom player effects like respecting level boundries that other sprites do not need to do.
void MisterX::move() {
    // perform various actions on request
    if (mJumping && !mDucking) jump();
    if (mDucking) duck();                       // ***DEBUG*** do something so can't duck in air? Unless we like ducking in the air.
    if (!mDucking && !mAttacking) {
        if (mWalkingLeft) moveLeft();
        if (mWalkingRight) moveRight();
    }
    if (mPunching) punch();

    // call Sprite move function to perform the actual movement that handles collision detection, etc
    Sprite::move();

    // make sure we haven't exceeded level bounds. Sprite class doesn't do this for us as enemies can leave level bounds.
    adjustForLevelBounds();
}