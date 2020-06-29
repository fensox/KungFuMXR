#include "Sprite.h"
#include <SDL.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>

// Constructor for a new Sprite object. Takes a filename string to the Sprite's data filename.
Sprite::Sprite(std::string pFilename) {
	mFilename = pFilename;
	mPosition.x = 0;
	mPosition.y = 0;

    load();
}

// returns the current texture to be rendered as a const reference to an SDL_Texture
SDL_Texture* Sprite::getTextureToRender() {
	return nullptr;
};

// returns current texture's collision rectangle as a const reference to an SDL_Rect
SDL_Rect* Sprite::getCollisionRect() {
	return nullptr;
};

// access function to get a reference to this sprite's position in 2D space
SDL_Point& Sprite::getPosition() {
	return mPosition;
};

// load sprite data from file - called from constructor
bool Sprite::load() {
    // First load in sprite meta data file. File read into hash table in the format: <std::string>(action mode name)=<vector<SDL Rect>>(sprite sheet clip rects)
    ClipsMap actionFrames{}; // see typedef in Sprite.h
    if (!loadDataFile(actionFrames)) {
        std::cerr << "Failed in Sprite::load. Sprite::loadDataFile returned false. Filename attempted was:" << mFilename << std::endl;
        return false;
    }
    
    //debug code - print ClipsMap to console
    for (std::pair<std::string, std::vector<SDL_Rect>> action : actionFrames ) {
        std::cout << "ClipsMap actionFrames[" << action.first << "] has " << action.second.size() << " animation frames.\n";
        for (int i{ 0 }; i < action.second.size(); ++i) {
            std::cout << "\t" << action.second[i].x << ", " << action.second[i].y << ", " << action.second[i].w << ", " << action.second[i].h << "\n";
        }
    }

    return true;
};

// Load the initial data file in with action mode names and clip rects for the sprite sheet. Store data in the passed in map and return boolean success.
bool Sprite::loadDataFile(ClipsMap& actionsFramesCount) {
    // attempt to open a filestream on the filename. Fail returning nullptr if not.
    std::ifstream fileStream{ mFilename };
    if (!fileStream) return false;
    
    // parse the file line by line storing key/values in the map. Lines beginning with # ignored as comments.
    std::string strInput{};
    std::string key{}, lastKey{};
    std::string value{};
    std::vector<SDL_Rect> clips{}; // holds all the sprite sheet clips for a given action
    while (std::getline(fileStream, strInput)) {
        // create a string stream from line we read in
        std::istringstream stream(strInput);
        if (std::getline(stream, key, '=')) {
            // if it is a comment skip
            if (key[0] == '#') continue;

            // we have the key now get the rest of the string and, using a helper function, turn comma delimited values into our SDL_Rect
            std::getline(stream, value); // get remaining string to right of = sign
            std::tuple<bool, SDL_Rect> tplRect = getRectFromCDV(value);
            if (std::get<0>(tplRect)) {               
                // check if action is same as last one. If so just add new SDL_Rect to vector otherwise clear the vector and start accumulating clips for a new action mode.
                if (key == lastKey) {
                    clips.push_back(std::get<1>(tplRect));
                } else {
                    // commit final key/value pair for last action mode we did before beginning a new vector for the new action mode we've discovered
                    actionsFramesCount[lastKey] = clips;
                    clips.clear();
                    lastKey = key;
                    clips.push_back(std::get<1>(tplRect));
                }
            } else {
                // helper funct tells us we failed parsing CDVs so output an error msg and return failure
                std::cerr << "Failed in Sprite::loadDataFile parsing comma delimited values from: " << mFilename << "\nSprite::getRectFromCDV returned false." << std::endl;
                return false; 
            }
        }
    }
    
    return true;
};

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
        rect.x = intRect[0];
        rect.y = intRect[1];
        rect.w = intRect[2];
        rect.h = intRect[3];
    }

    return std::tuple<bool, SDL_Rect> {success, rect};
};