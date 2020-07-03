#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include <tuple>
#include <memory>
#include "Texture.h"
#include "SDLMan.h"

class Sprite {

public:
	/* Constructor calls load() to load the sprite data in. Derived classes should alter const mMetaFilename and const mSpriteSheet members for their specific sprite data.
	 * Derived classes that supply their own constructor need to call this parent Sprite constructor to initialize and load the sprite correctly.
	 */
	Sprite(std::shared_ptr<SDLMan> sdlMan);
	Sprite() = delete;

	// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
	void render();

	/* Returns current texture's collision rectangle by value. Class Sprite creates a collision	box using the sprites
	position, height, and width. For more accurate collision this function should be overidden by derived classes. */
	SDL_Rect getCollisionRect();

	// Access function to get a reference to this sprite's position in 2D space as an SDL_Point object.
	SDL_Point& getPosition();

	// Access function to get the depth of this sprite as an int.
	int getDepth();

	// Access function to set the depth of this sprite as an int.
	void setDepth(int depth);

	// Returns the name of this sprite from the global mName constant.
	std::string getName();

	// Returns information about the Sprite object represented as a std::string for debugging purposes.
	virtual std::string toString();

protected:
	/**********************************************************************************
	 *         SPRITE SPECIFIC MEMBERS TO BE SET BY EACH DERIVED CLASS                *
	 *                                                                                *
	 * Two constant std::string members holding the paths and filenames of the        *
	 * sprite's data files relative to executable. Overridden by derived classes for  *
	 * each sprite type. The SDL_Color set's the transparency for the sprite sheet.   *
	 * The mName string is simply a name for the sprite (i.e. Ninja, Ghost, etc.)     *
	 * primarily used to identify debugging output. mScale is a multiplyer to scale   *
	 * the sprite by when rendering. Defaults below.                                  *
	 **********************************************************************************/
	std::string mMetaFilename		{ "data/thomas.dat" };
	std::string mSpriteSheet		{ "data/thomas.png" };
	SDL_Color mTrans				{ 255, 0, 255, 0 };
	std::string	mName				{ "Sprite" };
	int mScale						{ 1 };
	/**********************************************************************************/

	// A ClipsMap is a std::unordered_map container with the string name of an action (the key) mapped to a vector of SDL_Rect holding all sprite sheet clip information for that action.
	typedef std::unordered_map<std::string, std::vector<SDL_Rect>> ClipsMap;

	// Position of sprite.
	SDL_Point mPosition{};

	// String holding the current action mode this sprite is in.
	std::string mActionMode{"DEFAULT"};

	// int holding the current frame of animation for our action mode we are in.
	std::size_t mCurrentFrame{};

	// Unordered map to hold key/value pairs of action names (the key) and their animation frame coordinates on the sprite sheet (the value).
	ClipsMap mAnimMap{};

private:
	// Smart pointer to the Texture holding our sprite's sprite sheet.
	std::shared_ptr<Texture> mTexture{ nullptr };
	
	// Smart pointer to the SDLMan object passed in during construction.
	std::shared_ptr<SDLMan> mSDLMan{ nullptr };

	// Holds the depth of this Sprite. Used for rendering of things in front/behind each other.
	int mDepth{};

	// load sprite data from files (called from constructor). Return boolean success.
	bool load();

	// Load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile();

	// Load in the sprite sheet specified in the const string mSpriteSheet and set transparency. Return boolean success.
	bool loadSpriteSheet();

	// Helper function to split some of the file parsing work into smaller chunks. Takes a string of 4 int values, comma delimited, and returns an bool success and an SDL_Rect as a tuple.
	std::tuple<bool, SDL_Rect> getRectFromCDV(std::string pCDV);

	// Helper function to ge the width and height of an SDL_Texture. Returned in a SDL_Point type (x=width, y=height).
	SDL_Point getSize(Texture text);
};