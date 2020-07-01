#include "Sprite.h"
#include "FensoxUtils.h"
#include "SDLMan.h"
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

/* Constructor calls load() to load the sprite data in. Derived classes should alter const mMetaFilename and const mSpriteSheet members for their specific sprite data.
 * Derived classes that supply their own constructor need to call this parent Sprite constructor to initialize and load the sprite correctly.
 */
Sprite::Sprite(std::shared_ptr<SDLMan> sdlMan) {
    // store the SDLMan smart pointer
    mSDLMan = sdlMan;

    // default starting position
    mPosition.x = 0;
    mPosition.y = 0;

    // load in the data for our sprite
    load();
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

// load sprite data from file - called from constructor
bool Sprite::load() {
    // First load in sprite metadata file. Read it into a ClipsMap map (see header for typedef).
    if (!loadDataFile()) {
        std::cerr << "Failed in Sprite::load. Sprite::loadDataFile returned false. Filename attempted was:" << mMetaFilename << std::endl;
        return false;
    }

    //***DEBUG
    std::cout << toString();

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
            std::tuple<bool, SDL_Rect> tplRect = getRectFromCDV(value);
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

// Helper function to split some of the file parsing work into smaller chunks. Takes a string of 4 int values, comma delimited, and returns an bool success and an SDL_Rect as a tuple.
std::tuple<bool, SDL_Rect> Sprite::getRectFromCDV(std::string strCDV) {
    bool success{ true };
    int intRect[4]{ -1, -1, -1, -1 }; // temp array to hold SDL_Rect values. In a later check if any of these are left as -1 our load in failed.

    // convert string parameter to a stream and loop through grabbing int values and storing in temp array
    if (!strCDV.empty()) {
        std::istringstream stream(strCDV);
        std::string strTmp;
        int count{ 0 };
        while ( (std::getline(stream, strTmp, ',')) && (count < 4) ) {
            //convert substring we got into an int and catch any errors
            try {
                intRect[count] = std::stoi(strTmp);
                ++count;
            } catch (const std::exception& e) {
                std::cerr << "Failed in Sprite::getRectFromCDV converting str to int.\nError: " << e.what() << std::endl;
                success = false;
                break;
            }
        }
    } else {
        std::cerr << "Failed in Sprite::getRectFromCDV: function string parameter is empty." << std::endl;
        success = false;
    }

    // check if all four ints have been set
    if (success) {
        for (int i{ 0 }; i < 4; ++i) {
            if (intRect[i] == -1) {
                std::cerr << "Failed in Sprite::getRectFromCDV: Value " << i << " was -1 (meaning left unset or set incorrectly from file)." << std::endl;
                success = false;
            }
        }
    }

    // build our SDL_Rect
    SDL_Rect rect{};
    if (success) {
        rect = { intRect[0], intRect[1], intRect[2], intRect[3] };
    }

    return std::tuple<bool, SDL_Rect> {success, rect};
}

// Load in the sprite sheet specified in the const string mSpriteSheet. Return boolean success.
bool Sprite::loadSpriteSheet() {    
    mTexture = mSDLMan->loadImage(mSpriteSheet);
    return true;
}

// Returns object represented as a std::string for debugging purposes.
std::string Sprite::toString() {
    std::ostringstream output;
    for (std::pair<std::string, std::vector<SDL_Rect>> action : mAnimMap) {
        output << "ClipsMap actionFrames[" << action.first << "] has " << action.second.size() << " animation frames.\n";
        for (int i{ 0 }; i < action.second.size(); ++i) {
            output << "\t" << action.second[i].x << ", " << action.second[i].y << ", " << action.second[i].w << ", " << action.second[i].h << "\n";
        }
    }

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

// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
void Sprite::render() {
    // get our SDL_Rect for the current animation frame
    std::vector<SDL_Rect>& tmpVect = mAnimMap[mActionMode];
    SDL_Rect& clip = tmpVect[mCurrentFrame];
    
    // create a destination rect based on position and our frame size
    SDL_Rect dest { mPosition.x, mPosition.y, clip.w, clip.h };
    
    //Render to screen
    SDL_RenderCopyEx(   &mSDLMan->getRenderer(),
                        mTexture->getTexture(),
                        &clip,
                        &dest,
                        0,
                        NULL,
                        SDL_FLIP_NONE);
    
}