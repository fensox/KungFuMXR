#pragma once

#include "Level.h"
#include "Texture.h"
#include "SDLMan.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include <tuple>
#include <memory>

// Forward declaration for Level class...ran into a circular reference between Sprite<->Level
class Level;

class Sprite {

public:
	/* Constructor. Derived classes should have a constructor that fills in all their sprite specific variables. See section in Sprite.h.
	   After the Sprite derived is constructed a call to load() must be made before any other function calls will operate correctly.
	*/
	Sprite(std::shared_ptr<SDLMan> sdlMan);
	Sprite() = delete;

	~Sprite();

	// Load sprite data from files - Needs to be called before any other functions can be called.
	bool load();

	// Renders the sprite based on position, action mode, animation frame using a SDL_Renderer from SDLMan.
	void render();

	/*  Returns current Sprite's action frame collision rectangle by value. The position of the rectangle is set to player
	coordinates in the level. Width and height are set to the size of the sprite sheet animation we are currently on and
	scaled based on the Sprite mScale scaling factor. For more accurate	collision boxes than just the bounding box, this
	function may be overidden by derived classes. */
	virtual SDL_Rect getCollisionRect();

	// Sets the smart pointer member variable that points to the Level currently being played.
	void setLevel(std::shared_ptr<Level> level);

	// Access function to get the depth of this sprite as an int.
	int getDepth();

	// Access function to set the depth of this sprite as an int.
	void setDepth(int depth);

	// Returns the name of this sprite from the global mName constant.
	std::string getName();

	// Sets the sprite's x coordinate position relative to level.
	void setX(decimal x);

	// Sets the sprite's y coordinate position relative to level.
	void setY(decimal y);

	// Returns the sprite's x coordinate position relative to level.
	decimal getX();

	// Returns the sprite's y coordinate position relative to level.
	decimal getY();

	// Returns by value the current animation frame's rectangle. Sprite sheet coordinate relative.
	const SDL_Rect& getRect();

	// Draw collision points as crosshairs. Useful for debugging purposes.
	void drawCollisionPoints();

	// Moves Sprite based on velocities adjusting for gravity, friction, and collisions. May be overridden or extended for custom movement routines.
	virtual void move();

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
	 * the sprite by when rendering. The JUMP_VELOCITY constant is how much velocity  *
	 * in pixels to add to upward velocity when a sprite initiates a jump.			  *
	 * Defaults below.								                                  *
	 **********************************************************************************/
	std::string		mMetaFilename		{ "data/example.dat" };
	std::string		mSpriteSheet		{ "data/example_sheet.png" };
	std::string		mActionMode			{ "WALK_LEFT" };
	SDL_Color		mTrans				{ 255, 0, 255, 0 };
	std::string		mName				{ "Example Man" };
	int				mScale				{ 1 };
	const decimal	JUMP_VELOCITY		{ 4.5 };						// Initial force a sprite generates to start a jump in pixels per second
	/**********************************************************************************/

	// A ClipsMap is a std::unordered_map container with the string name of an action (the key) mapped to a vector of SDL_Rect holding all sprite sheet clip information for that action.
	typedef std::unordered_map<std::string, std::vector<SDL_Rect>> ClipsMap;

	// int holding the current frame of animation for our action mode we are in.
	std::size_t mCurrentFrame{};

	// Unordered map to hold key/value pairs of action names (the key) and their animation frame coordinates on the sprite sheet (the value).
	ClipsMap mAnimMap{};

	// Center position of sprite within the level. This is not viewport relative but level relative. This is not derived from
	// the size of the current animation frame. The render function renders the animation frame texture around this point.
	decimal mXPos{ 0 }; decimal mYPos{ 0 };

	// Advances the current action mode animation frame ahead or loops to beginning if at end of animation frames.
	void advanceFrame();

	// Smart pointer to the level we are on. Various Level functions allow sprites to move level viewport and
	// check level collision rectangles, boundries, etc..
	std::weak_ptr<Level> mLevel{ std::shared_ptr<Level>() };

	// Smart pointer to the SDLMan object passed in during construction.
	std::weak_ptr<SDLMan> mSDL{ std::shared_ptr<SDLMan>() };

	// Holds velocity/momentum for the four 2d directions. These modify speed/position in jumps, falls, etc.
	// Gravity, friction, hits taken, etc can also modify these in return.
	struct Velocity {
		decimal up{ 0 };
		decimal down{ 0 };
		decimal left{ 0 };
		decimal right{ 0 };
	};

	// Instance of Velocity struct for this sprite
	Velocity mVeloc{};

	// If we are actively walking left
	bool mWalkingLeft{ false };

	// If we are actively walking right
	bool mWalkingRight{ false };

	// If we are actively jumping
	bool mJumping{ false };

	// Last time we decreased walking velocity due to ground friction. Used to regulate gravity adjustments by time and not by frame as frame rates vary.
	Uint32 mLastFricTime{};

	// Handles check for collision downwards with level collision elements. Returns true if made contact with stable platform.
	bool downBump();

	// Handles the sprite initiating a jump.
	void jump();

	// Applies gravity to the sprite depending on boolean parameter. Also checks if just finished a fall and cleans up some variables if so.
	void applyGravity(bool standing);

	// Applies friction to the sprite's velocity.
	void applyFriction(bool standing);

private:
	// Smart pointer to the Texture holding our sprite's sprite sheet.
	std::unique_ptr<Texture> mTexture{ nullptr };

	// Holds the depth of this Sprite. Used for rendering of things in front/behind each other.
	int mDepth{};

	// Holds the destination rectangle we will be rendered into
	SDL_Rect mDest{};

	// The sprite's instance of the Velocity struct
	Velocity veloc{};

	// Load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile();

	// Load in the sprite sheet specified in the const string mSpriteSheet and set transparency. Return boolean success.
	bool loadSpriteSheet();

};