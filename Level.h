#pragma once

#include "Sprite.h"
#include "PointF.h"
#include "SDLMan.h"
#include <memory>
#include <vector>
#include <SDL.h>
#include <string>

// Forward declaration for Sprite class...ran into a circular reference between Sprite<->Level
class Sprite;

/*	
	Holds information about one level in the game. All levels viewed through a viewport whose
	size if specifided in FuGlobals::VIEWPORT_WIDTH & VIEWPORT_HEIGHT. This viewport is scaled to fit display resolution.
*/
class Level {
public:
	// Constructor takes path to metadata file for the level relative to game executable and an SDLMan pointer to hold for rendering.
	// Note load() must be called after construction of this object before other functions will work.
	Level(std::string filename, std::shared_ptr<SDLMan> sdlMan);

	// Destructor
	~Level();

	// Easier to work with typedef: A vector of SDL rectangle objects held by a smart pointer. Holds all hard collision objects for the level.
	typedef std::shared_ptr<std::vector<SDL_Rect>> ColRects;

	// Returns all hard collision objects in level.
	ColRects getColRects();

	// Checks if the given point is contained in a collision rectangle for the level.
	bool isACollision(const SDL_Point& pnt);

	// Checks if the given rectangle is intersecting a collision rectangle for the level.
	bool isACollision(const SDL_Rect& rect);

	// Checks if the given point is contained in a collision rect for the level. Parameter of PointF is cast to integer type SDL_Point.
	bool isACollision(PointF pnt);

	// Load in the data filefor the level. Must be called before other functions for proper operation. Returns success or failure.
	bool load();

	// Return player start position as an SDL_Point object
	SDL_Point getPlayStart();

	// Returns the width/height of the level background as an SDL_Point
	SDL_Point getSize();

	// Returns the viewport's top-left coordinates and width/height.
	SDL_Point getPosition();

	// Render the level
	void render();

	// Set's the Sprite pointer that this level's viewport will stay centered on.
	void setFollowSprite(std::shared_ptr<Sprite> follow);

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

	// Holds the current viewport rectangle over the level. Get's centered on mFollowSprite member's position.
	SDL_Point mViewport{ 0, 0 };

	// Smart pointer to an SDLMan object used to draw the level.
	std::weak_ptr<SDLMan> mSDLMan{ std::weak_ptr<SDLMan>() };

	// Smart pointer to a Sprite object this viewport will center on. Usually holds player sprite but could be any sprite i.e. a scripted scene, etc..
	std::weak_ptr<Sprite> mFollowSprite{ std::shared_ptr<Sprite>() };

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

	// Centers the viewport over the given coordinates adjusting for level boundries.
	void centerViewport();

	// Outlines all the collision rectangles in the level so visible on screen. Debugging and level design utility function.
	void drawColRects();
};