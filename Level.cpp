#include "Level.h"
#include <fstream>
#include <sstream>
#include "FensoxUtils.h"
#include <tuple>
#include <iostream>

// Constructor takes path to metadata file for the level relative to game executable and an SDLMan pointer to hold for rendering.
// Note load() must be called after construction of this object before other functions will work.
Level::Level(std::string filename, std::shared_ptr<SDLMan> sdlMan) {
	mMetaFile = filename;
    mSDLMan = sdlMan;
    mColRects = std::make_shared<std::vector<SDL_Rect>>();
}

// Loads in the level
bool Level::load() {
    bool success{ true };

    // Load in the level's metadata file into an unordered map
    if (!loadDataFile()) success = false;

    // Load in the level's music file
    if (!loadMusicFile()) success = false;

    // Load in the level's background texture
    if (!loadBGTexture()) success = false;
    
    return success;
}

// Load in the level's background texture. Returns success.
bool Level::loadBGTexture() {
    bool useTrans{ true };
    if (mTrans.a == 0) useTrans = false;
    mBGTexture = mSDLMan->loadImage(mBGFile, mTrans, useTrans);
    
    return (!(mBGTexture == nullptr));
}

// Load the level's music file into SDLMan.
bool Level::loadMusicFile() {
    return (mSDLMan->loadMusic(mMusicFile));
}

// Load in the level's metadata file
bool Level::loadDataFile() {
    bool success{ true };

    // attempt to open a filestream on the filename or return a failure.
    std::ifstream fileStream{ mMetaFile };
    if (!fileStream) return false;

    // parse the file line by line storing values in appropriate member variables. Lines beginning with # ignored as comments.
    std::string strInput{};
    std::string key{}, lastKey{ "#LASTKEY" }; // be sure it is unmatchable the first time (using comment character assures that)
    std::string value{};
    while (std::getline(fileStream, strInput)) {
        // create a string stream from line we read in
        std::istringstream stream(strInput);
        if (std::getline(stream, key, '=')) {
            // trim leading and trailing whitespace, uppercase, and if it is a comment skip
            FensoxUtils::strTrim(key);
            key = FensoxUtils::strToUpper(key);
            if (key[0] == '#' || key.empty()) continue;

            // we have the key now get the value and store different places depending on what the key is
            std::getline(stream, value);
            switch (FensoxUtils::hash(key.c_str())) {
            case FensoxUtils::hash("NAME"):
                mName = value;
                break;
            case FensoxUtils::hash("BACKGROUND"):
                mBGFile = value;
                break;
            case FensoxUtils::hash("MUSIC"):
                mMusicFile = value;
                break;
            case FensoxUtils::hash("PLAYER_START"):
                mPlayStart = FensoxUtils::getPointFromCDV(value);
                break;
            case FensoxUtils::hash("START_VIEWPORT"):
                if (!storeViewport(value)) success = false;
                break;
            case FensoxUtils::hash("TRANS_COLOR"):
                if (!storeTrans(value)) success = false;
                break;
            case FensoxUtils::hash("COLRECT"):
                if (!storeColRect(value)) success = false;
                break;
            default:
                std::cerr << "Warning in Level::load. Undefined key value in level metadata file. Key was: " << key;
                std::cerr << "\nAttempting to continue on reading metadata file." << std::endl;
            }
        }
    }

    return true;
}

// Helper function to take a comma delimited value from metadata file, convert to an SDL_Color, and store in our mTrans member. Returns success or failure.
bool Level::storeTrans(std::string value) {
    // Using a helper function, turn comma delimited values into an SDL_Rect
    std::tuple<bool, SDL_Rect> tplRect = FensoxUtils::getRectFromCDV(value);

    // check if we were successful in converting to an SDL_Rect
    if (std::get<0>(tplRect)) {
        SDL_Rect tmpRect = std::get<1>(tplRect);
        mTrans = { static_cast<Uint8>(tmpRect.x),
                   static_cast<Uint8>(tmpRect.y),
                   static_cast<Uint8>(tmpRect.w),
                   static_cast<Uint8>(tmpRect.h)
        };
    } else {
        // helper funct tells us we failed parsing CDVs so output an error msg and return failure
        std::cerr << "Failed in Level::storeTrans. FensoxUtils::getRectfromCDV returned false." << std::endl;
        return false;
    }

    return true;
}

// Helper function to take a comma delimited value, convert to an SDL_Rect, and store in our ColRects member. Returns success or failure.
bool Level::storeColRect(std::string value) {
    // Using a helper function, turn comma delimited values into an SDL_Rect
    std::tuple<bool, SDL_Rect> tplRect = FensoxUtils::getRectFromCDV(value);

    // check if we were successful in converting to an SDL_Rect
    if (std::get<0>(tplRect)) {
        mColRects->push_back(std::get<1>(tplRect));
    } else {
        // helper funct tells us we failed parsing CDVs so output an error msg and return failure
        std::cerr << "Failed in Level::storeColRect. FensoxUtils::getRectfromCDV returned false." << std::endl;
        return false;
    }

    return true;
}

// Helper function to take comma delimited value from metadata file, convert to an SDL_Rect, and store in our mViewport member. Returns success or failure.
bool Level::storeViewport(std::string value) {
    // Using a helper function, turn comma delimited values into an SDL_Rect
    std::tuple<bool, SDL_Rect> tplRect = FensoxUtils::getRectFromCDV(value);

    // check if we were successful in converting to an SDL_Rect
    if (std::get<0>(tplRect)) {
        mViewport = std::get<1>(tplRect);
    } else {
        // helper funct tells us we failed parsing CDVs so output an error msg and return failure
        std::cerr << "Failed in Level::storeViewport. FensoxUtils::getRectfromCDV returned false." << std::endl;
        return false;
    }

    return true;
}

// Return player start position
SDL_Point Level::getPlayStart() {
    return mPlayStart;
}

// Outputs the level information represented as a string
std::string Level::toString() {
    std::ostringstream str{};
    str << "Level Name: " << mName << ", Meta: " << mMetaFile << ", BG: " << mBGFile;
    str << "Music: " << mMusicFile << ", Player Start: " << mPlayStart.x << ", " << mPlayStart.y;
    str << ", # Collision Rectangles: " << mColRects->size() << std::endl;
    return str.str();
}

// Render the level to the screen
void Level::render() {
    // create a destination rect based on window size
    SDL_Rect dest{ 0, 0, mSDLMan->getWindowW() , mSDLMan->getWindowH() };

    //Render to screen
    SDL_RenderCopyEx(&mSDLMan->getRenderer(),
        mBGTexture->getTexture(),
        &mViewport,
        &dest,
        0,
        NULL,
        SDL_FLIP_NONE);
}

// Moves the viewport a distance in x/y pixels passed in using an SDL_Point. Returns the actual distance the viewport moved
// which can be less based on running into boundries, etc.
SDL_Point Level::moveViewport(const SDL_Point& dist) {
    // store the limit coordinates the background can move
    int limitX{ mBGTexture->getSize().x - mViewport.w };
    int limitY{ mBGTexture->getSize().y - mViewport.h };;

    // holds the move attempt temporarily for testing
    int tryX{ mViewport.x + dist.x };
    int tryY{ mViewport.y + dist.y };

    // test horizontal boundries of level
    if ((tryX < 0) || (tryX > limitX)) {
        if (tryX < 0) {
            // if less than 0 then we'll move all that is left in x coordinate to move
            tryX = mViewport.x;
        }
        else {
            // if we went beyond the width limit of the level then we'll move all that we could without going over the limit
            tryX = limitX - mViewport.x;
        }
    }

    // test vertical boundries of level
    if ((tryY < 0) || (tryY > limitY)) {
        if (tryY < 0) {
            // if less than 0 then we'll move all that is left in y coordinate to move
            tryY = mViewport.y;
        }
        else {
            // if we went beyond the height limit of the level then we'll move all that we could without going over the limit
            tryY = limitY - mViewport.y;
        }
    }

    // perform the actual move on the viewport
    mViewport.x = tryX;
    mViewport.y = tryY;

    // return what we actually moved not what was requested
    return SDL_Point{ tryX, tryY };
}