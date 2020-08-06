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
Sprite::Sprite(std::shared_ptr<SDLMan> sdlMan) {
    // store the SDLMan smart shared pointer in our weak_ptr. Using weak to prevent cyclic shared_ptr problems as multiple objects hold a reference to SDLMan.
    mSDL = sdlMan;

    // set default action mode set for this sprite into our action mode member
    mActionMode = mStartingActionMode;
}

// Destructor
Sprite::~Sprite() {
    if (FuGlobals::DEBUG_MODE) std::cerr << "Destructor: Sprite" << std::endl;
    mTexture.reset();
    mLevel.reset();
    mSDL.reset();
}

/*  Returns current Sprite's action frame collision rectangle by value. The position of the rectangle is set to player
    coordinates in the level. Position is not viewport compensated. Width and height are set to the size of the sprite
    sheet animation we are currently on and scaled based on the Sprite mScale scaling factor. */
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

/* Gets the collision rectangle from getCollisionRect() and returns a 2 pixel tall and 8 pixel width trimmed rectangle
   at the bottom of the current collision rectangle. Position not viewport compensated. Used for downBump collision
   detection, drawing debugging rectangles, etc. */
SDL_Rect Sprite::getCollRectBtm() {
    SDL_Rect rect{ getCollisionRect() };
    rect.y += static_cast<int>(rect.h / 2);
    rect.h = 2;
    rect.x += 4;
    rect.w -= 8;

    return rect;
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
void Sprite::setLevel(std::shared_ptr<Level> level) {
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

// Set the action mode
void Sprite::setActionMode(std::string actionMode) {
    mLastActionMode = mActionMode;
    mActionMode = actionMode;
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

int Sprite::getDepth() {
    return mDepth;
}

void Sprite::setDepth(int depth) {
    mDepth = depth;
}

// Sets the sprite's x coordinate position relative to level.
void Sprite::setX(decimal x) { mXPos = x; }

// Sets the sprite's y coordinate position relative to level.
void Sprite::setY(decimal y) { mYPos = y; }

// Returns the sprite's x coordinate position relative to level.
decimal Sprite::getX() { return mXPos; }

// Returns the sprite's y coordinate position relative to level.
decimal Sprite::getY() { return mYPos; }

// Returns the name of this sprite from the global mName constant.
std::string Sprite::getName() {
    return mName;
}

// Advances the current action mode animation frame ahead or loops to beginning if at end of animation frames.
void Sprite::advanceFrame() {
    // check if this is a new action mode since last frame change. If so store the new mode name and start at frame 0.
    if (mActionMode != mLastActionMode) {
        mCurrentFrame = 0;
        mLastActionMode = mActionMode;
    }

    // get the number of frames this animation has
    std::size_t count = mAnimMap[mActionMode].size();

    // increment the animation ahead or loop back to beginning if we reach the end of the frames available
    if (++mCurrentFrame >= count) mCurrentFrame = 0;
}

// Returns the current animation frame's rectangle from the sprite sheet. Sprite sheet coordinate relative.
const SDL_Rect& Sprite::getRect() {
    return mAnimMap[mActionMode].at(mCurrentFrame);
}

// Draws a mark on the screen for each collision point boundry. For debugging purposes.
void Sprite::drawCollisionPoints() {
    // set draw color and mark size
    mSDL.lock()->setDrawColor(255, 0, 0);
    int radius{ 3 };

    // draw a circle at our center coordinates
    mSDL.lock()->drawCircleFilled(static_cast<int>(mXPos - mLevel.lock()->getPosition().x), static_cast<int>(mYPos - mLevel.lock()->getPosition().y), radius);

    // draw lines at our collision edges
    SDL_Rect rect{ getCollRectBtm() };

    // draw bottom compensating for viewport position
    rect.x -= static_cast<int>(mLevel.lock()->getPosition().x + rect.w / 2);
    rect.y -= static_cast<int>(mLevel.lock()->getPosition().y - rect.h / 2);
    mSDL.lock()->drawLine(rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);

    // return draw color to black
    mSDL.lock()->setDrawColor(0, 0, 0);
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
   
    // adjust the Sprite coordinates by the distance the viewport is from origin (fixed a 3 day bug hunt where character exponentially ran faster than bg scroll)
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
    if (FuGlobals::DEBUG_MODE) drawCollisionPoints();
}

// Handles check for collision downwards with level collision elements. Returns true if made contact with stable platform.
bool Sprite::downBump() {
    // if we are rising up we don't need to do a downward collision check
    if (mVeloc.up > mVeloc.down) return false;

    return mLevel.lock()->isACollision( getCollRectBtm() );
}

// Applies gravity to the sprite if parameter set to true otherwise checks if sprite just finished a fall and cleans up velocity variables.
void Sprite::applyGravity(bool standing) {
    // If we are standing but have downward velocity still we have just landed. Reset y velocities to stop bouncing and other jump artifacts.
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
    // check for a downward collision to see if we are on stable ground. This will decide gravity and friction application
    bool standing{ downBump() };

    // apply gravity
    applyGravity(standing);

    // apply friction
    applyFriction(standing);

    // add up how much we are trying to move
    int tryX{ static_cast<int>(std::round(mVeloc.right - mVeloc.left)) };
    int tryY{ static_cast<int>(std::round(mVeloc.down - mVeloc.up)) };

    //get our collision rectangle and move x/y coordinates from center to top left
    SDL_Rect rect{ getCollisionRect() };
    rect.x -= rect.w / 2;
    rect.y -= rect.h / 2;

    // x test and possible reduction in distance
    if (tryX > 0) {                                 // going right
        for (int i{ 0 }; i < tryX; ++i) {
            rect.x += 1;
            if (mLevel.lock()->isACollision(rect)) {
                rect.x -= 1;
                break;
            }
        }
    } else if (tryX < 0) {                          // going left
        for (int i{ 0 }; i > tryX; --i) {
            rect.x -= 1;
            if (mLevel.lock()->isACollision(rect)) {
                rect.x += 1;
                break;
            }
        }
    }

    // y test and possible reduction in distance
    if (tryY > 0) {                                 // going down
        for (int i{ 0 }; i < tryY; ++i) {
            rect.y += 1;
            if (mLevel.lock()->isACollision(rect)) {
                rect.y -= 1;
                break;
            }
        }
    } else if (tryY < 0) {                          // going up - don't check for level collisions so we jump through platforms
        rect.y += tryY;
        // Old routine that detects platforms while velocity is taking us up. Commented out to allow jump through platforms plus was a little sticky on things.
        /*for (int i{ 0 }; i > tryY; --i) {
            rect.y -= 1;
            if (mLevel.lock()->isACollision(rect)) {
                rect.y += 1;
                break;
            }
        }*/
    }

    // set the position now that all tests have completed
    mXPos = rect.x + rect.w / 2;
    mYPos = rect.y + rect.h / 2;

    // Final check and fix that we didn't put the sprite somewhere in the floor becasue of animation frame size differences
    correctFrame();
}

// Corrects for height differences of various animation frames so we don't get stuck in floor, have jumpy animations, etc.
void Sprite::correctFrame() {
    // are we standing on the floor
    bool standing{ downBump() };

    // if we are standing on the floor do a test loop raising our position until we are above floor a pixel
    while (standing) {
        // move up one pixel and see if we are still standing
        mYPos -= 1;

        // check if we are standing on something now
        standing = downBump();

        // if not standing now then we are one pixel above the floor because of our testing, put it back to floor level and the loop will end
        if (!standing) mYPos += 1;
    }
}