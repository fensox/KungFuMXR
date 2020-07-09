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
    mScale = 3;
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

// Check if enough time has passed to walk then work with viewport position and move player if so
void Thomas::attemptWalk(int amount) {
    // we are walking - check if enough time has passed before taking a step
    Uint32 currentTime{ SDL_GetTicks() };
    if (currentTime - mLastWalkTime >= mWalkWaitTime) {
            advanceFrame();
            changePosition(SDL_Point{ amount, 0 });
            mLastWalkTime = currentTime;
    }
}

// Adjust the player position or the viewport position. This is called at end of a chain of function calls, begun at playerInput(), that have already confirmed
// enough time has passed to move, selected the appropriate animation frame, and now we are ready to adjust position of player or viewport depending
// on where the player is located within the viewport and position of the viewport within the level. The parameter is how much we are attempting to move on each axis
// and in what direction. We allow the player to move within the viewport only if he is not in the center of the viewport because of a boundry to a level. If a boundry
// to a level has been reached he is allowed to leave center of screen and get within FuGlobals::LEVEL_BOUNDS pixels of viewport.
void Thomas::changePosition(SDL_Point amount) {
    // Find out how far from the center of the viewport we are
    SDL_Rect curPos = getCenterPoint();
    SDL_Point far = mLevel->getDistFromViewCenter(SDL_Point{ curPos.x, curPos.y });

    //***DEBUG***
    if (FuGlobals::DEBUG_MODE) std::cout << "Thomas::changePosition() far.x: " << far.x << std::endl;

    // Handle moving left case
    if (amount.x < 0) {
        changePositionLeft(amount.x, far.x);
    } else {
        // Handle moving right case
        changePositionRight(amount.x, far.x);
    }
    
}

// Helper function for changePosition function to split some of the code into more manageable pieces. Handles moving left case.
void Thomas::changePositionLeft(int movX, int farX) {
    // Hold how far to move the player or viewport once calculations are done
    int playerX{}, viewX{};

    // If the player is to the right or equal to the center of the viewport
    if (farX <= 0) {
        // If the distance between the player and center of viewport is greater than our move distance just move the player
        if (farX <= movX) {
            playerX = movX;
        } else {
            // we can only move partially to the left before hitting center of viewport. We will move the player as much as we can then the viewport the rest.
            playerX = farX; // move player the final distance to center of viewport
            viewX = movX - playerX; // whatever distance is leftover from our move request distance move viewport
            
            // Ask the viewport to move as much as it can, if we hit edge of level move player remaining distance viewport coultn't move
            SDL_Point actual = mLevel->moveViewport(SDL_Point{ viewX, 0 });
            int diff{ abs(viewX) - abs(actual.x) };
            if (diff != 0) playerX -= diff;
            
            // The player has moved to left of viewport center so check we haven't exceeded LEVEL_BOUNDS margins. If so move us back to the margin coordinate.
            // This should only be called if the move speed is ridiculously fast where the player moves, in one frame, from center to beyond margin
            // Or if a level data file specifies a player starting position within the margin
            int tmpPosX{ getCenterPoint().x - playerX };
            if (tmpPosX < FuGlobals::LEVEL_BOUNDS) playerX += FuGlobals::LEVEL_BOUNDS - tmpPosX;
        }
    } else {
        // The player is to the left of the viewport center so we must have reached leftmost level bounds. Move player without moving viewport.
        playerX += movX;

        // check we haven't exceeded level margins set in LEVEL_BOUNDS global constant
        int tmpPosX{ getCenterPoint().x + playerX };
        if (tmpPosX < FuGlobals::LEVEL_BOUNDS) {
            playerX += FuGlobals::LEVEL_BOUNDS - tmpPosX;
        }
    }

    // Actually move player
    SDL_Rect currentPlayer{ getCenterPoint() };
    setCenterPoint(SDL_Point{ currentPlayer.x += playerX, currentPlayer.y } );
}

// Helper function for changePosition function to split some of the code into more manageable pieces. Handles moving left case.
void Thomas::changePositionRight(int movX, int farX) {
    // Hold how far to move the player or viewport once calculations are done
    int playerX{}, viewX{};

    // If the player is to the left or equal to the center of the viewport
    if (farX >= 0) {
        // If the distance between the player and center of viewport is greater than our move distance just move the player
        if (farX <= movX) {
            playerX = movX;
        } else {
            // we can only move partially to the right before hitting center of viewport. We will move the player as much as we can then the viewport the rest.
            playerX = farX; // move player the final distance to center of viewport
            viewX = movX - playerX; // whatever distance is leftover from our move request distance move viewport

            // Ask the viewport to move as much as it can, if we hit edge of level move player remaining distance viewport coultn't move
            SDL_Point actual = mLevel->moveViewport(SDL_Point{ viewX, 0 });
            int diff{ abs(viewX) - abs(actual.x) };
            if (diff != 0) playerX += diff; //***DEBUG*** Flippes from -= to += when pasting from changePositionLeft. Problem?

            // The player has moved to right of viewport center so check we haven't exceeded LEVEL_BOUNDS margins. If so move us back to the margin coordinate.
            // This should only be called if the move speed is ridiculously fast where the player moves, in one frame, from center to beyond margin
            // Or if a level data file specifies a player starting position within the margin
            int tmpPosX{ getCenterPoint().x + playerX };
            int rightBoundry{ FuGlobals::VIEWPORT_WIDTH - FuGlobals::LEVEL_BOUNDS };
            if (tmpPosX > rightBoundry) playerX = rightBoundry;
        }
    } else {
        // The player is to the right of the viewport center so we must have reached rightmost level bounds. Move player without moving viewport.
        playerX += movX;

        // check we haven't exceeded level margins set in LEVEL_BOUNDS global constant
        int tmpPosX{ getCenterPoint().x + playerX };
        int rightBoundry{ FuGlobals::VIEWPORT_WIDTH - FuGlobals::LEVEL_BOUNDS };
        if (tmpPosX > rightBoundry) playerX = rightBoundry;
    }

    // Actually move player
    SDL_Rect currentPlayer{ getCenterPoint() };
    setCenterPoint(SDL_Point{ currentPlayer.x += playerX, currentPlayer.y });
}

// Player control is handled by playerInput() but this checks for other influnces on the player. Namely velocities, gravity, attacks that needs to be factored in to position.
void Thomas::move() {
    // handle gravity
    
}