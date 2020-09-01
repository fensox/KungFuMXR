#include "Sprite.h"
#include "FensoxUtils.h"
#include "FuGlobals.h"
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <sstream>

/* Constructor. Derived classes should have a constructor that fills in all their sprite specific variables. See protected section in Sprite.h.
   After the Sprite derived is constructed a call to load() must be made before any other function calls will operate correctly.
*/
Sprite::Sprite(std::weak_ptr<SDLMan> sdlMan) {
    // store the SDLMan in our weak_ptr
    mSDL = sdlMan;

    // set default action mode set for this sprite into our action mode member
    mActionMode = mStartingActionMode;
}

// Destructor
Sprite::~Sprite() {
    if constexpr (FuGlobals::DEBUG_MODE) std::cerr << "Destructor: Sprite" << std::endl;
    mTexture.reset();
    mLevel.reset();
    mSDL.reset();
}

// Load sprite data from files - Needs to be called before any other functions can be called.
bool Sprite::load() {
    // First load in sprite metadata file. Read it into a ClipsMap map (see header for typedef).
    if (!loadDataFile()) {
        std::cerr << "Failed in Sprite::load. Sprite::loadDataFile returned false. Filename attempted was:" << mMetaFilename << std::endl;
        return false;
    }

    // Second load in the sprite textures from the sprite sheet using our recently aquired action names and animation clip coordinates into a AnimMap map (see header for typedef).
    if (!loadSpriteSheet()) {
        std::cerr << "Failed in Sprite::load. Sprite::loadActionAnims returned false. Filename attempted was:" << mSpriteSheet << std::endl;
        return false;    
    }

    return true;
}

// Load the initial data file in with action mode names and clip rects for the sprite sheet. Store data in member mAnimMap and return success.
bool Sprite::loadDataFile() {
    // attempt to open a filestream on the filename or return a failure.
    std::ifstream fileStream{ mMetaFilename };
    if (!fileStream) return false;
    
    // parse the file line by line storing key/values in the map. Lines beginning with # ignored as comments.
    std::string strInput{};
    std::string key{}, lastKey{"#LASTKEY"}; // be sure it is unmatchable the first time (using comment character assures that)
    std::string value{};
    std::vector<SDL_Rect> clips{}; // holds all the sprite sheet clips for a given action
    while (std::getline(fileStream, strInput)) {
        // create a string stream from line we read in
        std::istringstream stream(strInput);
        if (std::getline(stream, key, '=')) {
            // trim leading and trailing whitespace and if it is a comment skip
            FensoxUtils::strTrim(key);
            if (key[0] == '#' || key.empty()) continue;

            // we have the key now get the rest of the string and, using a helper function, turn comma delimited values into our SDL_Rect
            std::getline(stream, value); // get remaining string to right of = sign
            std::tuple<bool, SDL_Rect> tplRect = FensoxUtils::getRectFromCDV(value);
            if (std::get<0>(tplRect)) {               
                // check if action is same as last one. If so just add new SDL_Rect to vector otherwise clear the vector and start accumulating clips for a new action mode.
                if (key == lastKey) {
                    clips.push_back(std::get<1>(tplRect));
                } else {
                    // commit final key/value pair for last action mode we did before beginning a new vector for the new action mode we've discovered                    
                    if (lastKey == "#LASTKEY") lastKey = key; // handle first case
                    mAnimMap[lastKey] = clips;
                    clips.clear();
                    lastKey = key;
                    clips.push_back(std::get<1>(tplRect));
                }
                // perform final addition to ClipsMap not handled by our loop
                mAnimMap[lastKey] = clips;
            } else {
                // helper funct tells us we failed parsing CDVs so output an error msg and return failure
                std::cerr << "Failed in Sprite::loadDataFile parsing comma delimited values from: " << mMetaFilename << "\nSprite::getRectFromCDV returned false." << std::endl;
                return false; 
            }
        }
    }

    return true;
}

// Load in the sprite sheet specified in the const string mSpriteSheet and set transparency. Return boolean success.
bool Sprite::loadSpriteSheet() {    
    mTexture = mSDL.lock()->loadImage(mSpriteSheet);

    return (!(mTexture==nullptr));
}

// Sets the smart pointer member variable that points to the Level currently being played.
void Sprite::setLevel(std::weak_ptr<Level> level) {
    mLevel = level;
}

// Returns a string representation of the sprite information
std::string Sprite::toString() {
    const SDL_Rect& cr = getCollisionRect();
    std::ostringstream output;
    output << "Sprite name: " << mName << ", Position: " << mXPos << ", " << mYPos << ", Depth: " << mDepth << ", ";
    output << "# of action modes: " << mAnimMap.size() << ", " << "Current ation mode: " << mActionMode << "\n";
    output << "All clip rects for this mActionMode:\n";
    std::vector<SDL_Rect> tmpVect { mAnimMap[mActionMode] };
    for (int i{ 0 }; i < tmpVect.size(); ++i) {
        output << tmpVect.at(i).x << ", " << tmpVect.at(i).y << ", " << tmpVect.at(i).w << ", " << tmpVect.at(i).h << "\n";
    }

    return output.str();
}

// Set this sprite's health points.
void Sprite::setHealth(int health) {
    mHealth = health;
}

// Get this sprite's health points.
int Sprite::getHealth() {
    return mHealth;
}

// Set the action mode to enter into and also if it is a looping animation or not.
void Sprite::setActionMode(std::string actionMode, bool looping) {
    mLastActionMode = mActionMode;
    mLastActionModeLooping = mActionModeLooping;
    mActionMode = actionMode;
    mActionModeLooping = looping;
    mCurrentFrame = 0;
}

// Returns the current action mode
std::string Sprite::getActionMode() {
    return mActionMode;
}

// Returns the last action mode
std::string Sprite::getLastActionMode() {
    return mLastActionMode;
}

// Returns whethar the current action mode is a looping animation or not.
bool Sprite::getActionModeLooping() {
    return mActionModeLooping;
}

// Returns whethar the last action mode was a looping animation or not.
bool Sprite::getLastActionModeLooping() {
    return mLastActionModeLooping;
}

// Reverts action mode to the last action mode. Sets last action mode as mode we just changed out of. Swaps the two.
void Sprite::revertLastActionMode() {
    bool looping{ mActionModeLooping };
    std::string mode{ mActionMode };

    mActionMode = mLastActionMode;
    mActionModeLooping = mLastActionModeLooping;

    mLastActionMode = mode;
    mLastActionModeLooping = looping;

    mCurrentFrame = 0;
}

int Sprite::getDepth() {
    return mDepth;
}

void Sprite::setDepth(int depth) {
    mDepth = depth;
}

// Sets the sprite's x coordinate position relative to level and stores the previous coordinates as our last x position.
void Sprite::setX(decimal x) { 
    mLastXPos = mXPos;
    mXPos = x;
}

// Sets the sprite's y coordinate position relative to level and stores the previous coordinates as our last y position.
void Sprite::setY(decimal y) {
    mLastYPos = mYPos;
    mYPos = y;
}

// Returns the sprite's x coordinate position relative to level.
decimal Sprite::getX() { return mXPos; }

// Returns the sprite's y coordinate position relative to level.
decimal Sprite::getY() { return mYPos; }

// Returns the sprite's last x coordinate position relative to level.
decimal Sprite::getLastX() { return mLastXPos; }

// Returns the sprite's last y coordinate position relative to level.
decimal Sprite::getLastY() { return mLastYPos; }

// Returns the name of this sprite from the global mName constant.
std::string Sprite::getName() {
    return mName;
}

// Advances the current action mode animation frame ahead or loops to beginning if at end of animation frames and defined as a looping action mode.
void Sprite::advanceFrame() {
    // get the number of frames this animation has
    std::size_t totalFrames = mAnimMap[mActionMode].size();

    // increment the animation ahead
    ++mCurrentFrame;

    // if we are greater or equal to total frames either loop or set to last valid frame depending on if this is a looping animation
    if (mCurrentFrame >= totalFrames) {
        if (mActionModeLooping) mCurrentFrame = 0; 
        else mCurrentFrame = totalFrames - 1;
    }
}

// Returns the current animation frame's rectangle from the sprite sheet. Sprite sheet coordinate relative.
const SDL_Rect& Sprite::getRect() {
    return mAnimMap[mActionMode].at(mCurrentFrame);
}

// Set's the target Sprite object. Used in AI routines as the target sprite to follow/attack, etc.
void Sprite::setTargetSprite(std::weak_ptr<Sprite> targetSprite) {
    mTargetSprite = targetSprite;
}

// Draws a mark on the screen for each collision point boundry. For debugging purposes.
void Sprite::drawCollisionPoints() {
    // set draw color and mark size
    mSDL.lock()->setDrawColor(255, 255, 0);
    int radius{ 3 };

    // draw a circle at our center coordinates
    mSDL.lock()->drawCircleFilled(static_cast<int>(mXPos - mLevel.lock()->getPosition().x), static_cast<int>(mYPos - mLevel.lock()->getPosition().y), radius);

    // draw bottom
    Line line{ getVPRelative(getCollRectBtm()) };
    mSDL.lock()->drawLine(line);

    // draw left compensating for viewport position
    line = getVPRelative(getCollRectLeft());
    mSDL.lock()->drawLine(line);

    // draw right compensating for viewport position
    line = getVPRelative(getCollRectRight());
    mSDL.lock()->drawLine(line);

    // return draw color to black
    mSDL.lock()->setDrawColor(0, 0, 0);
}

/*  Returns current Sprite's action frame collision rectangle by value. The position of the rectangle is set to player
    coordinates in the level which places them in the center of the returned rectangle. Position is not viewport
    compensated. Width and height are set to the size of the sprite sheet animation we are currently on and scaled based
    on the Sprite mScale scaling factor. */
SDL_Rect Sprite::getCollisionRect() {
    // get current sprite sheet clip rectangle
    SDL_Rect rect{ mAnimMap[mActionMode].at(mCurrentFrame) };

    // adjust w, h based on our mScale scaling factor
    rect.w *= mScale;
    rect.h *= mScale;

    // adjust x, y coordinates from sprite sheet relative to level relative
    rect.x = static_cast<int>(mXPos);
    rect.y = static_cast<int>(mYPos);

    return rect;
}

// Returns a line representing the bottom of the current collision rectangle. Used for downBump collision detection, drawing debugging rectangles, etc.
Line Sprite::getCollRectBtm() {
    SDL_Rect rect{ getCollisionRect() };
    Line line{
        rect.x - rect.w / 2,
        rect.y + rect.h / 2,
        rect.x + rect.w / 2,
        rect.y + rect.h / 2
    };

    // Shrink the line to just a small centered segement. Prevents right/left collisions from seeming like floor collisions.
    int halfWidth = (line.x2 - line.x1) / 2;
    line.x1 += halfWidth - 2;
    line.x2 -= halfWidth + 2;

    return line;
}

// Returns a line representing the left side of the current collision rectangle. Used for leftBump collision detection, drawing debugging rectangles, etc.
Line Sprite::getCollRectLeft() {
    SDL_Rect rect{ getCollisionRect() };
    Line line{
        rect.x - rect.w / 2,
        rect.y - rect.h / 2,
        rect.x - rect.w / 2,
        rect.y + rect.h / 2
    };

    // Shrink line a pixel from top and bottom to prevent floors from seeming as collisions
    ++line.y1;
    --line.y2;

    return line;
}

// Returns a line representing the right side of the current collision rectangle. Used for rightBump collision detection, drawing debugging rectangles, etc.
Line Sprite::getCollRectRight() {
    SDL_Rect rect{ getCollisionRect() };
    Line line{
        rect.x + rect.w / 2,
        rect.y - rect.h / 2,
        rect.x + rect.w / 2,
        rect.y + rect.h / 2
    };

    // Shrink line a pixel from top and bottom to prevent floors from seeming as collisions
    ++line.y1;
    --line.y2;

    return line;
}

// Takes a rectangle with level relative coordinates and converts them to viewport relative. Returns a copy of the rectangle with updated coordinates.
SDL_Rect Sprite::getVPRelative(const SDL_Rect& inRect) {
    SDL_Rect outRect{ inRect };
    outRect.x -= mLevel.lock()->getPosition().x;
    outRect.y -= mLevel.lock()->getPosition().y;

    return outRect;
}

// Takes a line with level relative coordinates and converts them to viewport relative. Returns a copy of the line with updated coordinates.
Line Sprite::getVPRelative(const Line& inLine) {
    Line outLine{ inLine };
    outLine.x1 -= mLevel.lock()->getPosition().x;
    outLine.y1 -= mLevel.lock()->getPosition().y;
    outLine.x2 -= mLevel.lock()->getPosition().x;
    outLine.y2 -= mLevel.lock()->getPosition().y;

    return outLine;
}

// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
void Sprite::render() {
    // get our SDL_Rect for the current animation frame
    const SDL_Rect& clip{ getRect() };

    // scale our animation frame based on mScale member for this sprite
    decimal scaledW = clip.w * mScale;
    decimal scaledH = clip.h * mScale;

    // create a destination rect cenetering texture on our position
    decimal x{ mXPos - (scaledW / 2) };
    decimal y{ mYPos - (scaledH / 2) };
    mDest = { static_cast<int>(x), static_cast<int>(y), static_cast<int>(scaledW), static_cast<int>(scaledH) };
   
    // adjust the Sprite coordinates to viewport relative
    mDest.x -= mLevel.lock()->getPosition().x;
    mDest.y -= mLevel.lock()->getPosition().y;

    //Render to screen
    SDL_RenderCopyEx(   mSDL.lock()->getRenderer(),
                        mTexture->getTexture(),
                        &clip,
                        &mDest,
                        0,
                        NULL,
                        SDL_FLIP_NONE);

    //***DEBUG*** Draw collision points as crosshairs if debug global is on.
    if constexpr (FuGlobals::DEBUG_MODE) drawCollisionPoints();
}

// Check for collision downwards with level collision elements. Returns true if made contact with stable platform.
bool Sprite::downBump() {
    // if we are rising up we don't need to do a downward collision check
    if (mVeloc.up > mVeloc.down) return false;

    return mLevel.lock()->isACollision( getCollRectBtm() );
}

// Check for right side collisions with level elements. Returns true if made contact with a collidable level object.
bool Sprite::rightBump() {
    return mLevel.lock()->isACollision( getCollRectRight() );
}

// Checks if a right side collision is imminent 1 pixel beyond Sprite's collision boundry.
bool Sprite::rightBumpImminent() {
    Line rightLine{ getCollRectRight() };
    ++rightLine.x1;
    ++rightLine.x2;
    return mLevel.lock()->isACollision(rightLine);
}

// Checks for left side collisions with level elements. Returns true if made contact with a collidable level object.
bool Sprite::leftBump() {
    return mLevel.lock()->isACollision( getCollRectLeft() );
}

// Checks if a left side collision is imminent 1 pixel beyond Sprite's collision boundry.
bool Sprite::leftBumpImminent() {
    Line leftLine{ getCollRectLeft() };
    --leftLine.x1;
    --leftLine.x2;
    return mLevel.lock()->isACollision( leftLine );
}

// Applies gravity to the sprite if parameter set to true otherwise checks if sprite just finished a fall and cleans up velocity variables.
void Sprite::applyGravity(bool standing) {
    // If we are standing but have downward velocity still, we have just landed. Reset y velocities to stop bouncing and other jump artifacts.
    if (standing && (mVeloc.down > 0)) {
        mVeloc.up = 0;
        mVeloc.down = 0;
    } else if (!standing) {
        // we are falling, apply proper amount of gravity depending on framerate timing to hit our real world GRAVITY constant
        decimal gravThisFrame{ FuGlobals::GRAVITY / mSDL.lock()->getFPS() };
        mVeloc.up -= gravThisFrame;
        if (mVeloc.up < 0) mVeloc.up = 0;
        mVeloc.down += gravThisFrame;

        // adjust to be sure we don't exceed terminal velocity
        decimal tvThisFrame{ FuGlobals::TERMINAL_VELOCITY / mSDL.lock()->getFPS() };
        if (mVeloc.down > tvThisFrame) mVeloc.down = tvThisFrame;
    }
}

// Applies friction to the sprite to slow horizontal movement. Handles surface and air friction depending on bool parameter true of false respectively.
void Sprite::applyFriction(bool standing) {
        // set what friction value we will use and divide it by current FPS average to get our pixels per real world second
        decimal friction{ FuGlobals::GROUND_FRICTION };
        if (!standing) friction = FuGlobals::AIR_FRICTION;
        friction = friction / mSDL.lock()->getFPS();

        // apply the friction being sure velocity not reduced below 0
        mVeloc.left -= friction;
        if (mVeloc.left < 0) mVeloc.left = 0;

        mVeloc.right -= friction;
        if (mVeloc.right < 0) mVeloc.right = 0;
}

// Moves Sprite based on velocities adjusting for gravity, friction, and collisions. May be overridden or extended for custom movement routines.
void Sprite::move() {
    // check for a downward collision to see if we are on stable ground. This will affect gravity and friction application.
    bool standing{ downBump() };

    // apply gravity
    applyGravity(standing);

    // apply friction
    applyFriction(standing);

    // add up how much we are trying to move and make the change to our position
    setX( getX() + mVeloc.right - mVeloc.left );
    setY( getY() + mVeloc.down - mVeloc.up );

    // Final check for level collisions to remove the sprite from any floors or walls
    correctFrame();
}

//***DEBUG*** 
// This correctFrame function can be optimized using lastXPos and lastYPos to determine which way we moved since last frame and allow us
// to avoid doing collision detection in the other direction. May not matter that much but an option once game is complete.

// After all movement for frame is calculated and implemented, adjusts Sprite position based on any collisions.
void Sprite::correctFrame() {
    bool colliding{};

    // Correct for downward collision if we moved down this frame
    colliding = downBump();
    while (colliding) {
        // move up one pixel and see if we are still standing
        mYPos -= 1;
        colliding = downBump();

        // if not standing now then we are one pixel above a surface because of our testing, put it back to surface level and the loop will end
        if (!colliding) mYPos += 1;
    }

    // correct for right side intersection
    colliding = rightBump();
    while (colliding) {
        // move left one pixel and see if we are still colliding
        mXPos -= 1;
        colliding = rightBump();
    }

    // correct for left side intersection
    colliding = leftBump();
    while (colliding) {
        // move left one pixel and see if we are still colliding
        mXPos += 1;
        colliding = leftBump();
    }
}