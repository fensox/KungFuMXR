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

    //***DEBUG***
    if (FuGlobals::DEBUG_MODE) std::cout << toString();

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

// Returns all hard collision objects in level as our typedef'd ColRects type.
Level::ColRects Level::getColRects() {
    return mColRects;
}

// Return player start position
SDL_Point Level::getPlayStart() {
    return mPlayStart;
}

// Returns the viewport on the levels top-left coordinates and the viewport width/height.
SDL_Rect Level::getPosition() {
    return mViewport;
}

// Outputs the level information represented as a string
std::string Level::toString() {
    std::ostringstream str{};
    str << "Level Name: " << mName << ", Meta: " << mMetaFile << ", BG: " << mBGFile;
    str << ", Music: " << mMusicFile << ", Player Start: " << mPlayStart.x << ", " << mPlayStart.y;
    str << ", # Collision Rectangles: " << mColRects->size() << "\nCollision Rectangles List:";
    for (int i{ 0 }; i < mColRects->size(); ++i) {
        str << "\n" << mColRects->at(i).x << ", " << mColRects->at(i).y << ", " << mColRects->at(i).w << ", " << mColRects->at(i).h;
    }
    str << std::endl;
    return str.str();
}

// Returns the height and width of the level background in an SDL_Point.
SDL_Point Level::getSize() {
    return mSDLMan->getSize(mBGTexture);
}

// Set's the Sprite pointer that this level's viewport will stay centered on.
void Level::setFollowSprite(std::shared_ptr<Sprite> follow) {
    mFollowSprite = follow;
}

// Render the level to the screen
void Level::render() {    
    // center viewport on the Sprite we've been told to follow
    centerViewport();

    //Render the area of level our viewport is pointing at
    SDL_RenderCopyEx(mSDLMan->getRenderer(),
        mBGTexture->getTexture(),
        &mViewport,
        NULL,
        0,
        NULL,
        SDL_FLIP_NONE);

    //***DEBUG*** if debug on draw all collision rectangles so visible on screen
    if (FuGlobals::DEBUG_MODE) drawColRects();
}

// Centers the viewport on given x, y coordinates adjusting for level boundries.
void Level::centerViewport() {
    // move the viewport to given coordinates
    mViewport.x = static_cast<int>(mFollowSprite->getX() - (FuGlobals::VIEWPORT_WIDTH / 2));
    mViewport.y = static_cast<int>(mFollowSprite->getY() - (FuGlobals::VIEWPORT_HEIGHT / 2));
    
    // check against level boundries and move back some if need be
    int limitX{ mBGTexture->getSize().x - mViewport.w };
    int limitY{ mBGTexture->getSize().y - mViewport.h };
    // x
    if (mViewport.x > limitX) mViewport.x = limitX;
    else if (mViewport.x < 0) mViewport.x = 0;
    // y
    if (mViewport.y > limitY) mViewport.y = limitY;
    else if (mViewport.y < 0) mViewport.y = 0;
}

// Outlines all the collision rectangles in the level so visible on screen. Debugging and level design utility function.
void Level::drawColRects() {
    // Set drawing color and loop through all collision rectangles drawing them
    mSDLMan->setDrawColor(0, 255, 0);
    for (int i{ 0 }; i < mColRects->size(); ++i) {
        SDL_Rect r = mColRects->at(i);   
        r.x -= mViewport.x; // compensate for viewport's distance from level origin
        r.y -= mViewport.y;
        SDL_RenderDrawRect(mSDLMan->getRenderer(), &r);
    }

    // Return our draw color to black
    mSDLMan->setDrawColor(0, 0, 0, 255);
}

// Checks if the given point is contained in a collision rect for the level.
bool Level::isACollision(SDL_Point pnt) {
    // loop through all our level collision rectangles checking for a collision
    for (int i{ 0 }; i < mColRects->size(); ++i) {
        SDL_Rect r = mColRects->at(i);
        r.x -= mViewport.x; // compensate for viewport's distance from level origin
        r.y -= mViewport.y;
        if (SDL_PointInRect(&pnt, &r)) return true;
    }

    return false;
}

// Checks if the given point is contained in a collision rect for the level. Parameter of PointF is cast to integer type SDL_Point.
bool Level::isACollision(PointF pnt) {
    return isACollision( pnt.getSDL_Point() );
}