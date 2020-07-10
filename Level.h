#pragma once

#include <memory>
#include <vector>
#include <SDL.h>
#include <string>
#include "SDLMan.h"

/*	
	Holds information about one level in the game. All levels have a 1280 x 720 pixel viewport that scales to fit display.
	Levels are 720 pixels tall graphic files and we view them through a 1280x720 viewport.
*/
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

	// Returns the width/height of the level background in an SDL_Point.
	SDL_Point getSize();

	// Returns the top/left coordinate of the viewport on the level.
	SDL_Point getPosition();

	// Render the level
	void render();

	// Centers the viewport over the given coordinates adjusting for level boundries.
	void centerViewport(int x, int y);

	// Outputs the object information represented as a string
	std::string toString();

private:
	// Holds the path and filename to the level's metadata file
	std::string mMetaFile{};

	// Holds the name of the level as loaded from metadata
	std::string mName{};

	// Holds the path and filename to the level's music file
	std::string mMusicFile{};

	// Holds the path and filename to the level's background graphic
	std::string mBGFile{};

	// Holds the SDL_Texture of the level background in a smart pointer
	std::shared_ptr<Texture> mBGTexture{ nullptr };

	// Holds the transparency color for the background texture
	SDL_Color mTrans{};

	// Player starting coordinates as an SDL_Point in reference to the center of player
	SDL_Point mPlayStart{};

	// Holds the current viewport rectangle over the level. Get's centered on player position.
	SDL_Rect mViewport{0, 0, FuGlobals::VIEWPORT_WIDTH, FuGlobals::VIEWPORT_HEIGHT};

	// Smart pointer to an SDLMan object used to draw the level
	std::shared_ptr<SDLMan> mSDLMan{ nullptr };

	// Holds all collision rectangles in a vector wrapped in a smart pointer.
	ColRects mColRects{nullptr};

	// Helper function to take a comma delimited value, convert to an SDL_Rect, and store in our ColRects member. Returns success or failure.
	bool storeColRect(std::string value);

	// Helper function to take a comma delimited value from metadata file, convert to an SDL_Color, and store in our mTrans member. Returns success or failure.
	bool storeTrans(std::string value);

	// Load in the level's metadata file. Returns success.
	bool loadDataFile();

	// Load in the level's background texture. Returns success.
	bool loadBGTexture();

	// Load in the level's music file. Returns Success.
	bool loadMusicFile();
};