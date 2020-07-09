#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include <tuple>
#include <memory>
#include "Texture.h"
#include "SDLMan.h"
#include "Level.h"

class Sprite {

public:
	/* Constructor. Derived classes should have a constructor that fills in all their sprite specific variables. See section in Sprite.h.
	   After the Sprite derived is constructed a call to load() must be made before any other function calls will operate correctly.
	*/
	Sprite(std::shared_ptr<SDLMan> sdlMan);
	Sprite() = delete;

	// Load sprite data from files - Needs to be called before any other functions can be called.
	bool load();

	// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
	void render();

	/* Returns current texture's collision rectangle by value. Class Sprite creates a collision	box using the sprites
	position, height, and width. For more accurate collision this function should be overidden by derived classes. */
	SDL_Rect getCollisionRect();

	// Access function to get a const reference to the center point of this sprite's position in 2D space as an SDL_Rect object.
	SDL_Rect getCenterPoint();

	// Access function to set the center point of this sprite's position in 2D space as an SDL_Point object.
	void setCenterPoint(SDL_Point newCtr);

	// Sets the smart pointer member variable that points to the Level currently being played.
	void setLevel(std::shared_ptr<Level> level);

	// Access function to get the depth of this sprite as an int.
	int getDepth();

	// Access function to set the depth of this sprite as an int.
	void setDepth(int depth);

	// Returns the name of this sprite from the global mName constant.
	std::string getName();

	// Set's the position of the sprite.
	void setStartPosition(SDL_Point start);

	// Handle movement of the sprite. Purely virtual function as all sprites move differently.
	virtual void move() = 0;

	// Returns information about the Sprite object represented as a std::string for debugging purposes.
	virtual std::string toString();

protected:
	/**********************************************************************************
	 *         SPRITE SPECIFIC MEMBERS TO BE SET BY EACH DERIVED CLASS                *
	 *                                                                                *
	 * Two constant std::string members holding the paths and filenames of the        *
	 * sprite's data files relative to executable. Overridden by derived classes for  *
	 * each sprite type. The mActionMode needs to be set to the starting action mode  *
	 * for the sprite. The SDL_Color set's the transparency for the sprite sheet.     *
	 * The mName string is simply a name for the sprite (i.e. Ninja, Ghost, etc.)     *
	 * primarily used to identify debugging output. mScale is a multiplyer to scale   *
	 * the sprite by when rendering. Defaults below.                                  *
	 **********************************************************************************/
	std::string mMetaFilename		{ "data/example.dat" };
	std::string mSpriteSheet		{ "data/example_sheet.png" };
	std::string mActionMode			{ "WALK_LEFT" };
	SDL_Color mTrans				{ 255, 0, 255, 0 };
	std::string	mName				{ "Example Man" };
	int mScale						{ 1 };
	/**********************************************************************************/

	// A ClipsMap is a std::unordered_map container with the string name of an action (the key) mapped to a vector of SDL_Rect holding all sprite sheet clip information for that action.
	typedef std::unordered_map<std::string, std::vector<SDL_Rect>> ClipsMap;

	// int holding the current frame of animation for our action mode we are in.
	std::size_t mCurrentFrame{};

	// Unordered map to hold key/value pairs of action names (the key) and their animation frame coordinates on the sprite sheet (the value).
	ClipsMap mAnimMap{};

	// Advances the current action mode animation frame ahead or loops to beginning if at end of animation frames.
	void advanceFrame();

	// Holds velocity/momentum for the four 2d directions. These modify speed/position in jumps, falls, etc.
	// Gravity, friction, hits taken, etc can also modify these in return.
	struct velocity {
		int up{ 0 };
		int down{ 0 };
		int left{ 1 };
		int right{ 0 };
	};

	// Smart pointer to the level we are on. Various Level functions allow sprites to move level viewport and
	// check level collision rectangles, boundries, etc..
	std::shared_ptr<Level> mLevel{ nullptr };

private:
	// Position of sprite.
	SDL_Point mPosition{ 0, 0 };

	// Smart pointer to the Texture holding our sprite's sprite sheet.
	std::shared_ptr<Texture> mTexture{ nullptr };
	
	// Smart pointer to the SDLMan object passed in during construction.
	std::shared_ptr<SDLMan> mSDLMan{ nullptr };

	// Holds the depth of this Sprite. Used for rendering of things in front/behind each other.
	int mDepth{};

	// Load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile();

	// Load in the sprite sheet specified in the const string mSpriteSheet and set transparency. Return boolean success.
	bool loadSpriteSheet();
};