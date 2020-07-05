#pragma once

#include <memory>
#include <vector>
#include <SDL.h>
#include <string>
#include "SDLMan.h"

/* Holds information about one level in the game. */
class Level {
public:
	// Constructor takes path to metadata file for the level relative to game executable and an SDLMan pointer to hold for rendering.
	// Note load() must be called after construction of this object before other functions will work.
	Level(std::string filename, std::shared_ptr<SDLMan> sdlMan);

	// Easier to work with typedef: A vector of SDL rectangle objects held by a smart pointer. Holds all hard collision objects for the level.
	typedef std::shared_ptr<std::vector<SDL_Rect>> ColRects;

	// Returns all hard collision objects in level.
	ColRects getColRects();

	// Load in the data filefor the level. Must be called before other functions for proper operation. Returns success or failure.
	bool load();

	// Return player start position
	SDL_Point getPlayStart();

	// Outputs the object information represented as a string
	std::string toString();

private:
	// Holds the path and filename to the level's metadata file
	std::string mMetaFile{};

	// Holds the name of the level as loaded from metadata
	std::string mName{};

	// Holds the path and filename to the level's music file
	std::string mMusicFile{};

	// Holds the path and filename to the level's background graphic (720 pixels tall graphics for a 1280x720 viewport)
	std::string mBGFile{};

	// Player starting coordinates as an SDL_Point to the BOTTOM-RIGHT of player iamge
	SDL_Point mPlayStart{};

	// Smart pointer to an SDLMan object used to draw the level
	std::shared_ptr<SDLMan> mSDLMan{ nullptr };

	// Holds all collision rectangles in a vector wrapped in a smart pointer.
	ColRects mColRects{nullptr};

	// Helper function to take a comma delimited value, convert to an SDL_Rect, and store in our ColRects member. Returns success or failure.
	bool storeColRect(std::string value);
};