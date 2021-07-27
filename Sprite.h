#pragma once

#include "Level.h"
#include "Texture.h"
#include "SDLMan.h"
#include "Line.h"
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
	Sprite(std::weak_ptr<SDLMan> sdlMan);
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

	// Returns a line representing the bottom of the current collision rectangle. Used for downBump collision detection, drawing debugging rectangles, etc.
	Line getCollRectBtm();

	// Returns a line representing the left side of the current collision rectangle. Used for leftBump collision detection, drawing debugging rectangles, etc.
	Line getCollRectLeft();

	// Returns a line representing the right side of the current collision rectangle. Used for rightBump collision detection, drawing debugging rectangles, etc.
	Line getCollRectRight();

	// Returns a line representing the bottom of the current collision rectangle. Used for downBump collision detection, drawing debugging rectangles, etc.
	Line getCollRectTop();

	// Takes a rectangle with level relative coordinates and converts them to viewport relative. Returns a copy of the rectangle with updated coordinates.
	SDL_Rect getVPRelative(const SDL_Rect& inRect);

	// Takes a line with level relative coordinates and converts them to viewport relative. Returns a copy of the line with updated coordinates.
	Line getVPRelative(const Line& inLine);

	// Sets the smart pointer member variable that points to the Level currently being played.
	void setLevel(std::weak_ptr<Level> level);

	// Set's the target Sprite object. Used in AI routines as the target sprite to follow/attack, etc.
	void setTargetSprite(std::weak_ptr<Sprite> targetSprite);

	// Access function to get the depth of this sprite as an int.
	int getDepth();

	// Access function to set the depth of this sprite as an int.
	void setDepth(int depth);

	// Returns the name of this sprite from the global mName constant.
	std::string getName();

	// Sets the sprite's x coordinate position relative to level and stores the previous coordinates as our last x position.
	void setX(decimal x);

	// Sets the sprite's y coordinate position relative to level and stores the previous coordinates as our last y position.
	void setY(decimal y);

	// Returns the sprite's x coordinate position relative to level.
	decimal getX();

	// Returns the sprite's y coordinate position relative to level.
	decimal getY();

	// Returns the sprite's last x coordinate position relative to level.
	decimal getLastX();

	// Returns the sprite's last y coordinate position relative to level.
	decimal getLastY();

	// Returns by value the current animation frame's rectangle. Sprite sheet coordinate relative.
	const SDL_Rect& getRect();

	// Moves Sprite based on velocities adjusting for gravity, friction, and collisions. May be overridden or extended for custom movement routines.
	virtual void move();

	// Set the action mode to enter into and also if it is a looping animation or not.
	void setActionMode(std::string actionMode, bool looping);

	// Returns the current action mode
	std::string getActionMode();

	// Returns the last action mode
	std::string getLastActionMode();

	// Returns whethar the current action mode is a looping animation or not.
	bool getActionModeLooping();

	// Returns whethar the last action mode was a looping animation or not.
	bool getLastActionModeLooping();

	// Reverts action mode to the last action mode. Sets last action mode as mode we just changed out of. Swaps the two.
	void revertLastActionMode();

	// Set this sprite's health points.
	void setHealth(int health);

	// Get this sprite's health points.
	int getHealth();

	// Adjusts the sprite's health points by the given amount
	void adjustHealth(int amount);

	// Set this sprite's maximum health points.
	void setHealthMax(int healthMax);

	// Get this sprite's maximum health points.
	int getHealthMax();

	// Returns information about the Sprite object represented as a std::string for debugging purposes.
	virtual std::string toString();

protected:
	//**DEBUG** Get rid of this and make a constructor that takes all of it so we could potential load it all from a file and not have to hard
	//code it all. Either constructor takes all these parameters or it takes a struct that holds them all.

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

	// Indicates if we are in the middle of an attack.
	bool mAttacking{ false };

	// Indicates if we already caused damage during this attack (prevents one attack anim from causing multiple strikes)
	bool mAttackDmgDone{ false };

	// Advances the current action mode animation frame ahead or starts at the beginning if at end of animation frames or a new action has been started.
	void advanceFrame();

	// Smart pointer to the level we are on. Various Level functions allow sprites to move level viewport and
	// check level collision rectangles, boundries, etc..
	std::weak_ptr<Level> mLevel;

	// The Sprite object the AI will use as a target. Used for program controlled sprite's move function AI.
	std::weak_ptr<Sprite> mTargetSprite;

	// Smart pointer to the SDLMan object passed in during construction.
	std::weak_ptr<SDLMan> mSDL;

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

	// Collision detection function. Paramaters are:
	//		enum ColType inType: what to check for a collision with: level geometry or other sprites
	//		enum ColDirect inDirect: indicates direction to check for collision
	//		int inPixels: distance in pixels to check for a collision. i.e. value of 0 is an actual collision, a value of 1 would mean a collision is 1 pixel away
	// 	    std::shared_ptr<Sprite> colSprite: Optional parameter to be filled w/ pointer to the Sprite we collided with.
	// Return value is a whethar a collision is true
	bool isCollision(FuGlobals::ColType inType, FuGlobals::ColDirect inDirect, int inPixels, std::weak_ptr<Sprite> &colSprite);
	bool isCollision(FuGlobals::ColType inType, FuGlobals::ColDirect inDirect, int inPixels);

	// Applies gravity to the sprite depending on boolean parameter. Also checks if just finished a fall and cleans up some variables if so.
	void applyGravity(bool standing);

	// Applies friction to the sprite's velocity.
	void applyFriction(bool standing);

	// Check's for health reaching 0 and begins death animation.
	virtual void processDeath();

private:
	// Center position of sprite within the level. This is not viewport relative but level relative. This is not derived from
	// the size of the current animation frame. The render function renders the animation frame texture around this point.
	decimal mXPos{ 0 }; decimal mYPos{ 0 };

	// Last position sprite was in before current position.
	decimal mLastXPos{ 0 }; decimal mLastYPos{ 0 };

	// Smart pointer to the Texture holding our sprite's sprite sheet.
	std::unique_ptr<Texture> mTexture{ nullptr };

	// Maximum health points of the Sprite.
	int mHealthMax{ 100 };

	// Health points of the Sprite. Zero means dead.
	int mHealth{ 100 };

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

	// Is the current action mode a looping animation or not
	bool mActionModeLooping{ false };

	// Is the last action mode a looping animation or not
	bool mLastActionModeLooping{ false };

	// Load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile();

	// Load in the sprite sheet specified in the const string mSpriteSheet and set transparency. Return boolean success.
	bool loadSpriteSheet();

	// Draw collision points as crosshairs. Useful for debugging purposes.
	void drawCollisionPoints();
	
	// After all movement for frame is made, adjust for any collisions with level geometry.
	void correctFrameLevel();

	// After movement for frame made, adjust for any collisions with other sprites
	void correctFrameSprites();

};