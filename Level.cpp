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

// Loads in the levels metadata file into an unordered_map container.
bool Level::load() {
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
            switch ( FensoxUtils::hash( key.c_str() ) ) {
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
                case FensoxUtils::hash("COLRECT"):
                    storeColRect(value);
                    break;
                default:
                    std::cerr << "Warning in Level::load. Undefined key value in level metadata file. Key was: " << key;
                    std::cerr << "\nAttempting to continue on reading metadata file." << std::endl;
            }
        }
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