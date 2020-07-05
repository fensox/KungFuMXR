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

    //***DEBUG***
    FensoxUtils::strToUpper("test of strToUpper function from Sprite constructor. hello.");
}

/* Returns current texture's collision rectangle by value. Class Sprite creates a collision	box using the sprites
position, height, and width. For more accurate collision this function should be overidden by derived classes. */
SDL_Rect Sprite::getCollisionRect() {
    SDL_Rect colRect{};
    /*
    SDL_Point tmpPoint = getSize(getTextureToRender());
    colRect.x = mPosition.x;
    colRect.y = mPosition.y;
    colRect.w = tmpPoint.x;
    colRect.h = tmpPoint.y;
    */
    return colRect;
}

// Access function to get a reference to this sprite's position in 2D space as an SDL_Point object.
SDL_Point& Sprite::getPosition() {
	return mPosition;
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
    mTexture = mSDLMan->loadImage(mSpriteSheet, mTrans);
    return true;
}

// Returns a string representation of the sprite information
std::string Sprite::toString() {
    SDL_Rect cr = getCollisionRect();
    std::ostringstream output;
    output << "Sprite name: " << mName << ", Position: " << mPosition.x << ", " << mPosition.y << ", Depth: " << mDepth << ", ";
    output << "# of action modes: " << mAnimMap.size() << ", "<< "Current ation mode: " << mActionMode << ", Collision rect: ";
    output << cr.x << ", " << cr.y << ", " << cr.w << ", " << cr.h << std::endl;

    return output.str();
}

// Helper function to ge the width and height of the current animation frame Texture. Returned in an SDL_Point type (x=width, y=height).
SDL_Point Sprite::getSize(Texture text) {
    SDL_Point size{};
    SDL_QueryTexture(text.getTexture(), NULL, NULL, &size.x, &size.y);
    return size;
}

int Sprite::getDepth() {
    return mDepth;
}

void Sprite::setDepth(int depth) {
    mDepth = depth;
}

// Returns the name of this sprite from the global mName constant.
std::string Sprite::getName() {
    return mName;
}

// Set's the position of the sprite.
void Sprite::setStartPosition(SDL_Point start) {
    int startWidth{ mAnimMap[mActionMode].at(0).w };
    int startHeight{ mAnimMap[mActionMode].at(0).h };
    mPosition.x = mSDLMan->getWindowW() - (startWidth * mScale);
    mPosition.y = mSDLMan->getWindowH() - (startHeight * mScale);
}

// Advances the current action mode animation frame ahead or loops to beginning if at end of animation frames.
void Sprite::advanceFrame() {
    // get the number of frames this animation has
    std::size_t count = mAnimMap[mActionMode].size();

    // increment the animation ahead or loop back to beginning if we reach the end of the frames available
    if (++mCurrentFrame >= count) mCurrentFrame = 0;
}

// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
void Sprite::render() {
    // get our SDL_Rect for the current animation frame
    std::vector<SDL_Rect>& tmpVect = mAnimMap[mActionMode];
    SDL_Rect& clip = tmpVect[mCurrentFrame];
    
    // create a destination rect based on position, and our frame size multiplied by mScale factor member
    SDL_Rect dest { mPosition.x, mPosition.y, clip.w * mScale, clip.h * mScale };
    
    //Render to screen
    SDL_RenderCopyEx(   &mSDLMan->getRenderer(),
                        mTexture->getTexture(),
                        &clip,
                        &dest,
                        0,
                        NULL,
                        SDL_FLIP_NONE);
    
}