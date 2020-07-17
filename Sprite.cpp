#include "Sprite.h"
#include "FensoxUtils.h"
#include "SDLMan.h"
#include "Globals.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>
#include <memory>
#include <vector>

/* Constructor. Derived classes should have a constructor that fills in all their sprite specific variables. See section in Sprite.h.
   After the Sprite derived is constructed a call to load() must be made before any other function calls will operate correctly.
*/
Sprite::Sprite(std::shared_ptr<SDLMan> sdlMan) {
    // store the SDLMan smart pointer
    mSDLMan = sdlMan;
}

/* Returns current texture's collision rectangle by value. Class Sprite creates a collision	box using the sprites
position, height, and width. For more accurate collision this function should be overidden by derived classes. */
const SDL_Rect& Sprite::getCollisionRect() {
    return  mAnimMap[mActionMode].at(mCurrentFrame);
}

/* Returns the current collision rectangle's center bottom point. */
SDL_Point Sprite::getCollisionRectBottom() {
    const SDL_Rect r = getCollisionRect();
    return SDL_Point { r.x, r.y + (r.h/2) };
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
    mTexture = mSDLMan->loadImage(mSpriteSheet, mTrans, true);
    
    return (!(mTexture==nullptr));
}

// Sets the smart pointer member variable that points to the Level currently being played.
void Sprite::setLevel(std::shared_ptr<Level> level) {
    mLevel = level;
}

// Returns a string representation of the sprite information
std::string Sprite::toString() {
    SDL_Rect cr = getCollisionRect();
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
    // get the number of frames this animation has
    std::size_t count = mAnimMap[mActionMode].size();

    // increment the animation ahead or loop back to beginning if we reach the end of the frames available
    if (++mCurrentFrame >= count) mCurrentFrame = 0;
}

// Returns the current animation frame's rectangle.
SDL_Rect Sprite::getRect() {
    return mAnimMap[mActionMode].at(mCurrentFrame);
}

// Draw a mark on four collision point boundries for debugging purposes.
void Sprite::drawCollisionPoints() {
    // set crosshair draw color to orange
    mSDLMan->setDrawColor(255, 255, 0);

    //bottom
    SDL_Point p{ getCollisionRectBottom() };
    p.x += mXPos - mLevel->getPosition().x;
    p.y += mYPos - mLevel->getPosition().y;
    mSDLMan->drawCircle(p.x, p.y, 100);

    
    // return draw color to black
    mSDLMan->setDrawColor(0, 0, 0);
}

// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
void Sprite::render() {
    // get our SDL_Rect for the current animation frame
    std::vector<SDL_Rect>& tmpVect = mAnimMap[mActionMode];
    SDL_Rect& clip{ tmpVect[mCurrentFrame] };

    // scale our animation frame based on mScale member for this sprite
    double scaledW = clip.w * mScale;
    double scaledH = clip.h * mScale;

    // create a destination rect cenetering texture on our position
    double x{ mXPos - (scaledW / 2) };
    double y{ mYPos - (scaledH / 2) };
    mDest = { static_cast<int>(x), static_cast<int>(y), static_cast<int>(scaledW), static_cast<int>(scaledH) };
   
    // adjust the Sprite coordinates by the distance the viewport is from origin (fixed a 3 day bug hunt where character exponentially ran faster than bg scroll)
    mDest.x -= mLevel->getPosition().x;
    mDest.y -= mLevel->getPosition().y;

    //Render to screen
    SDL_RenderCopyEx(   mSDLMan->getRenderer(),
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
    // get our current action frame clip rectangle, and test if a pixel one beneath it collides with the level
    SDL_Rect rect{ getRect() };
    SDL_Point pnt{ rect.x, static_cast<int>(mYPos + (rect.h / 2) + 1) };

    return mLevel->isACollision( pnt );
}

// Applies gravity to the sprite depending on boolean parameter. Also checks if just finished a fall and cleans up some variables if so.
void Sprite::applyGravity(bool standing) {
    if (standing) {
        // we are standing on something, if its the end of a fall, stop and downward velocity AND upward velocity to make sure up/down both canceled out.
        // Stops a bouncing player effect if they have not evenly canceled each other out upon landing.
        if (mVeloc.down != 0) {
            mVeloc.up = 0;
            mVeloc.down = 0;
        }
    } else {
        // we are falling. If enough time has passed apply some gravity to our downward velocity.
        if (SDL_GetTicks() - mLastGravTime >= FuGlobals::GRAVITY_TIME) {
            mVeloc.up -= FuGlobals::GRAVITY;
            if (mVeloc.up < 0) mVeloc.up = 0;
            mVeloc.down += FuGlobals::GRAVITY;
            mLastGravTime = SDL_GetTicks();
        }
    }
}

// Applies friction to the sprite
void Sprite::applyFriction() {
    mVeloc.left -= FuGlobals::FRICTION;
    if (mVeloc.left < 0) mVeloc.left = 0;
    mVeloc.right -= FuGlobals::FRICTION;
    if (mVeloc.right < 0) mVeloc.right = 0;
}

// Moves Sprite based on velocities adjusting for gravity, friction, and collisions. May be overridden or extended for custom movement routines.
void Sprite::move() {
    // check for a downward collision to see if we are on stable ground. This will decide gravity and friction application
    bool standing{ downBump() };

    // apply gravity - if not downBump'ing increase down velocity by our gravity global every gravity time interval specified by our gravity globals
    applyGravity(standing);

    // apply friction
    if (standing) applyFriction();

    // temporarily adjust position based on velocities then we test if we can move that much without a collision
    decimal tryX = mXPos + mVeloc.right - mVeloc.left;
    decimal tryY = mYPos - mVeloc.up + mVeloc.down;

    // x test and possible reduction in distance
    SDL_Rect rect{ getRect() };
    int tmpPos = static_cast<int>(tryX);
    PointF pnt{ 0.f, 0.f };
    if (tryX > 0) {
        for (int i{ tmpPos }; i > 0; --i) {
            pnt = { i + (rect.w / 2), tryY };
            if (!mLevel->isACollision(pnt)) break;
        }
    } else if (tryX < 0) {
        for (int i{ tmpPos }; i < 0; ++i) {
            pnt = { i - (rect.w / 2), tryY };
            if (!mLevel->isACollision(pnt)) break;
        }
    }

    // y test and possible reduction in distance
    tmpPos = static_cast<int>(tryY);
    if (tryY > 0) {
        for (int i{ tmpPos }; i > 0; --i) {
            pnt = { tryX, i + (rect.h / 2) };
            if (!mLevel->isACollision(pnt)) break;
        }
    } else if (tryY < 0) {
        for (int i{ tmpPos }; i < 0; ++i) {
            pnt = { tryX, i - (rect.h / 2) };
            if (!mLevel->isACollision(pnt)) break;
        }
    }

    // set the position now that all tests have completed
    mXPos = tryX;
    mYPos = tryY;
}