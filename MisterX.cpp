#include "MisterX.h"
#include "FensoxUtils.h"
#include "FuGlobals.h"
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

    // load sound effects
    mSDL.lock()->addSoundEffect("MRX_PUNCH", "data/mrx_punch.wav");
    mSDL.lock()->addSoundEffect("MRX_KICK", "data/mrx_kick.wav");
};

//***DEBUG*** Outputs some debugging info
void MisterX::outputDebug() {
    std::cout << "Player: " << getX() << "\t" << getY() << "\t" << getRect().w * mScale << "\t" << getRect().h * mScale << "\n";
    std::cout << "P Veloc L, R / U, D: " << mVeloc.left << "\t" << mVeloc.right << "\t/\t" << mVeloc.up << "\t" << mVeloc.down << "\n";
    std::cout << "Collision Rect: " << getCollisionRect().x << "\t" << getCollisionRect().y << "\t" << getCollisionRect().w << "\t" << getCollisionRect().h << "\n";
    std::cout << "mActionMode: " << getActionMode() << "\tmLastActionMode: " << getLastActionMode() << "\tmCurrentFrame: " << mCurrentFrame << "\n";
    std::cout << "Downbump:" << std::boolalpha << downBump() << "\tFPS: " << mSDL.lock()->getFPS() << "\n";
}

// Handles jopystick input from the player.
void MisterX::handleInputAnalogStick(const SDL_ControllerAxisEvent e) {
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
        } else {                                                            // Stick not engaged on y axis
            mDucking = false;
        }
    }
}

// Handles gamepad button input from the player.
void MisterX::handleInputGamepad(const SDL_ControllerButtonEvent e, bool press) {
    switch (e.button) {
        case SDL_CONTROLLER_BUTTON_A:
            if(press && !mJumping && !mDucking) mJumping = true;
            break;
        case SDL_CONTROLLER_BUTTON_B:
            if (press && mAttackReleased) {
                mAttacking = true;
                mKicking = true;
                mPunching = false;
                mAttackReleased = false;
            } else if (!press) {
                mAttackReleased = true;
            }
            break;
        case SDL_CONTROLLER_BUTTON_X:
            if (press && mAttackReleased) {
                mAttacking = true;
                mPunching = true;
                mKicking = false;
                mAttackReleased = false;
            } else if (!press) {
                mAttackReleased = true;
            }
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            // Y button
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            // dpad up
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            mDucking = press;
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
void MisterX::handleInputKeyboard(const SDL_Keycode& key, bool press) {
    switch (key) {
        case SDLK_SLASH:
            //***DEBUG***
            if (press) outputDebug();
            break;
        case SDLK_SPACE:
            if (press && !mJumping && !mDucking) mJumping = true;
            break;
        case SDLK_DOWN:
            mDucking = press;
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
                mKicking = false;
                mAttackReleased = false;
            } else if (!press) {
                mAttackReleased = true;
            }
            break;
        case SDLK_d:
            if (press && mAttackReleased) {
                mAttacking = true;
                mKicking = true;
                mPunching = false;
                mAttackReleased = false;
            } else if (!press) {
                mAttackReleased = true;
            }
            break;
    }
}

// Handles the player requesting to move to the right.
void MisterX::moveRight() {
    if (mDucking || mAttacking || !mWalkingRight) return;

    // if the previous action was different set new mActionMode, set animation frame to 0, and don't move player position
    if ( (getActionMode() != "WALK_RIGHT") && (getActionMode() != "JUMP_RIGHT") ) {
        mFacingRight = true;
        if (!downBump()) {
            setActionMode("JUMP_RIGHT");
        } else {
            setActionMode("WALK_RIGHT");
        }        
    } else {
        // if we have no vertical velocity make sure we are not in the jump animation
        if (mVeloc.down == 0 && mVeloc.up == 0 && getActionMode().compare("WALK_RIGHT") != 0) {
            setActionMode("WALK_RIGHT");
        }

        // step animation frame if enough time has passed and we're not pressed up against an object
        if ( checkWalkTime() && !rightBumpImminent() ) advanceFrame();

        // increase velocity based on FPS calc to reach our per second goal
        mVeloc.right += WALK_VELOCITY_PER / mSDL.lock()->getFPS();
        if (mVeloc.right > WALK_MAX) mVeloc.right = WALK_MAX;
    }
}

// Handles the player requesting to move to the left.
void MisterX::moveLeft() {
    if ( mDucking || mAttacking || !mWalkingLeft) return;

    // if the previous action was different set new mActionMode, mCurrentFrame 0, and don't move player position
    if ( (getActionMode() != "WALK_LEFT") && (getActionMode() != "JUMP_LEFT") ) {
        mFacingRight = false;
        if (!downBump()) {
            setActionMode("JUMP_LEFT");
        } else {
            setActionMode("WALK_LEFT");
        }
    } else if (!leftBump()) {
        // if we have no vertical velocity make sure we are not in the jump animation
        if (mVeloc.down == 0 && mVeloc.up == 0 && getActionMode().compare("WALK_LEFT") != 0) {
            setActionMode("WALK_LEFT");
        }

        // step animation frame if enough time has passed and we're not pressed up against an object
        if ( checkWalkTime() && !leftBumpImminent() ) advanceFrame();

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
    if (getX() < FuGlobals::LEVEL_BOUNDS) setX( FuGlobals::LEVEL_BOUNDS );
    else if (getX() > rightBound) setX( rightBound );

    // y
    if (getY() < FuGlobals::LEVEL_BOUNDS) setY( FuGlobals::LEVEL_BOUNDS );
    else if (getY() > downBound) setY( downBound );
}

// Handles player initiating a jump.
void MisterX::jump() {
    if (mDucking || !mJumping) return;

    // only launch into a jump if we have something to launch off of
    if (downBump()) {
        // check if we are already in the jump animation. Which means we are just landing not taking off
        if ( getActionMode().compare("JUMP_RIGHT") == 0 ) {
            setActionMode("WALK_RIGHT");
            mJumping = false;
        } else if ( getActionMode().compare("JUMP_LEFT") == 0 ) {
            setActionMode("WALK_LEFT");
            mJumping = false;
        } else {
            // we weren't finishing a jump so we will start one - change into jump animation
            if (mFacingRight) {
                setActionMode("JUMP_RIGHT");
            } else {
                setActionMode("JUMP_LEFT");
            }

            // increase our upward velocity
            mVeloc.down = 0;
            mVeloc.up = JUMP_VELOCITY; 
            //***DEBUG*** need to adjust above to be FPS bases so jump height doesn't change depending on performance! Like moveRight and moveLeft
            // ... work's if framerate is capped at 120fps but on a slow device that can't make 120fps jump height will be smaller
        }
    }
}

// Handles the player punching. mPunching bool not tied to button release like other actions. We turn off when animation complete to end punching action.
void MisterX::punch() {
    using namespace FensoxUtils;

    if (!mPunching || mKicking) return;

    // If start of punch action set our attack start time and play sound effect
    if (getActionMode().find("PUNCH_") == std::string::npos) {
        mSDL.lock()->playSoundEffect("MRX_PUNCH");
        mAttackTime = SDL_GetTicks();
    }
    
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
        case (hash("JUMP_RIGHT")):
            setActionMode("PUNCH_JUMP_RIGHT");
            break;
        case (hash("JUMP_LEFT")):
            setActionMode("PUNCH_JUMP_LEFT");
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

// Handles the player kicking. mKicking bool not tied to button release like other actions. We turn off when animation complete to end punching action.
void MisterX::kick() {
    using namespace FensoxUtils;

    if (!mKicking || mPunching) return;

    // If start of kick action set our attack start time and play sound effect
    if (getActionMode().find("KICK_") == std::string::npos) {
        mSDL.lock()->playSoundEffect("MRX_KICK");
        mAttackTime = SDL_GetTicks();
    }

    // if not in kick mode yet pick correct kick mode
    switch (hash(getActionMode().c_str())) {
        case (hash("DUCK_RIGHT")):
            setActionMode("KICK_DUCK_RIGHT");
            break;
        case (hash("DUCK_LEFT")):
            setActionMode("KICK_DUCK_LEFT");
            break;
        case (hash("WALK_RIGHT")):
            setActionMode("KICK_RIGHT");
            break;
        case (hash("WALK_LEFT")):
            setActionMode("KICK_LEFT");
            break;
        case (hash("JUMP_LEFT")):
            setActionMode("KICK_JUMP_LEFT");
            break;
        case (hash("JUMP_RIGHT")):
            setActionMode("KICK_JUMP_RIGHT");
            break;
    }

    // stay in kick mode animation until ATTACK_TIME has passed
    Uint32 time{ SDL_GetTicks() - mAttackTime };
    if ((time) >= ATTACK_TIME) {
        setActionMode(getLastActionMode());
        mKicking = false;
        mAttacking = false;
    }
}

// Handles the player initiating or coming out of a duck action
void MisterX::duck() {
    if (mDucking) {
        if (mFacingRight) {
            setActionMode("DUCK_RIGHT");
        } else {
            setActionMode("DUCK_LEFT");
        }
    } else {
        // restore walking action mode
        if (mFacingRight) {
            setActionMode("WALK_RIGHT");
        } else {
            setActionMode("WALK_LEFT");
        }
    }
}

// Moves player based on velocities adjusting for gravity, friction, and collisions. Extends then calls the Sprite class
// default move function for a few custom player effects like respecting level boundries that other sprites do not need to do.
void MisterX::move() {
    jump();                 // Handle any jumping
    
    moveLeft();             // Handle any requests to move left

    moveRight();            // Handle any requests to move right

    punch();                // Handle any requests to punch

    kick();                 // Handle any requests to kick

    duck();                 // Handle any requests to duck

    Sprite::move();         // call Sprite move function to perform actual movement, handling collision detection, etc

    adjustForLevelBounds(); // Check player hasn't exceeded level bounds. Sprite class doesn't do this for us as other Sprites can leave level bounds.
}