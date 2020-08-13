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

/* Provides basic Sprite functionality. Loading in sprite sheet meta info, animations, collision detection, basic movement with gravity, friction, and boundry checking. 
 * Includes some basic actions like walking and jumping. Derived sprite classes can build on these basic actions. */
class Sprite {

public:
	/* Constructor. Derived classes should have a constructor that fills in all their sprite specific variables. See protected section in Sprite.h.
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
	scaled based on the Sprite mScale scaling factor. */
	SDL_Rect getCollisionRect();

	// Returns a 2 pixel tall and 8 pixel width trimmed rectangle at the bottom of the current collision rectangle. Used for downBump collision detection, drawing debugging rectangles, etc.
	SDL_Rect getCollRectBtm();

	// Returns a 2 pixel wide rectangle at the left side of the current collision rectangle. Used for leftBump collision detection, drawing debugging rectangles, etc.
	SDL_Rect getCollRectLeft();

	// Returns a 2 pixel wide rectangle at the right side of the current collision rectangle. Used for leftBump collision detection, drawing debugging rectangles, etc.
	SDL_Rect getCollRectRight();

	// Takes a rectangle with level relative coordinates and converts them to viewport relative. Returns a copy of the rectangle with updates coordinates.
	SDL_Rect getVPRelative(const SDL_Rect& inRect);

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

	// Moves Sprite based on velocities adjusting for gravity, friction, and collisions. May be overridden or extended for custom movement routines.
	virtual void move();

	// Set the action mode
	void setActionMode(std::string actionMode);

	// Returns the current action mode
	std::string getActionMode();

	// Returns the last action mode
	std::string getLastActionMode();

	// Returns information about the Sprite object represented as a std::string for debugging purposes.
	virtual std::string toString();

protected:
	/**********************************************************************************
	 *         SPRITE SPECIFIC MEMBERS TO BE SET BY EACH DERIVED CLASS                *
	 **********************************************************************************/

	std::string		mMetaFilename		{ "data/example.dat" };			// The path to the sprite meta data file. See meta data file header for file layout information.
	std::string		mSpriteSheet		{ "data/example_sheet.png" };	// The path to the sprite sheet containing the animation frames for this sprite.
	std::string		mStartingActionMode	{ "WALK_LEFT" };				// The starting action mode for the sprite.
	SDL_Color		mTrans				{ 255, 0, 255, 0 };				// The transparency for the sprite sheet. Derived sprites may choose to auto detect transparency color or use this color.
	std::string		mName				{ "Example Man" };				// A name for the sprite (i.e.Ninja, Ghost, etc.) primarily used to identify debugging output.
	int				mScale				{ 1 };							// multiplyer to scale the sprite size by when rendering.
	bool			mFacingRight		{ false };						// On start of game indicates whethar the sprite is facing right or not. If false then sprite is facing left.

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

	// Advances the current action mode animation frame ahead or starts at the beginning if at end of animation frames or a new action has been started.
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

	// Handles check for collision downwards with level collision elements. Returns true if made contact with stable platform.
	bool downBump();

	// Handles check for right side collisions with level elements. Returns true if made contact with a collidable level object.
	bool rightBump();

	// Applies gravity to the sprite depending on boolean parameter. Also checks if just finished a fall and cleans up some variables if so.
	void applyGravity(bool standing);

	// Applies friction to the sprite's velocity.
	void applyFriction(bool standing);

	// Corrects for height differences of various animation frames so we don't get stuck in floor, have jumpy animations, etc.
	void correctFrame();

private:
	// Smart pointer to the Texture holding our sprite's sprite sheet.
	std::unique_ptr<Texture> mTexture{ nullptr };

	// Holds the depth of this Sprite. Used for rendering of things in front/behind each other.
	int mDepth{};

	// Holds the destination rectangle we will be rendered into
	SDL_Rect mDest{};

	// The sprite's instance of the Velocity struct
	Velocity veloc{};

	// The current action mode for the sprite.
	std::string mActionMode{};

	// The last action mode we were in before the current one or an empty string if beginning of Sprite life.
	std::string mLastActionMode{};

	// Load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile();

	// Load in the sprite sheet specified in the const string mSpriteSheet and set transparency. Return boolean success.
	bool loadSpriteSheet();

	// Draw collision points as crosshairs. Useful for debugging purposes.
	void drawCollisionPoints();

};