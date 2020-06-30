#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include <tuple>
#include <memory>
#include "Texture.h"

class Sprite {

public:
	// Constructor that takes in sprite data filename and calls load() to load the sprite in. Default constructor deleted.
	Sprite(std::string pFilename);
	Sprite() = delete;

	// Returns by reference the current texture to be rendered.
	Texture& getTextureToRender();

	/* Returns current texture's collision rectangle by value. Class Sprite creates a collision	box using the sprites
	position, height, and width. For more accurate collision this function should be overidden by derived classes. */
	SDL_Rect getCollisionRect();

	// Access function to get a reference to this sprite's position in 2D space as an SDL_Point object.
	SDL_Point& getPosition();

protected:
	// A ClipsMap is a std::unordered_map container with the string name of an action (the key) mapped to a vector of SDL_Rect holding all sprite sheet clip information for that action.
	typedef std::unordered_map<std::string, std::vector<SDL_Rect>> ClipsMap;
	
	// An AnimMap is a std::unordered_map container with the string name of an action (the key) mapped to a vector of SDL_Texture holding all animation frames for that action.
	typedef std::unordered_map<std::string, std::vector<Texture>> AnimMap;

	// Position of sprite.
	SDL_Point mPosition;

	// String holding the current action mode this sprite is in.
	std::string mActionMode;

	// int holding the current frame of animation for our action mode we are in.
	std::size_t mCurrentFrame{};

	// Unordered map to hold key/value pairs of action names (the key) and their animation textures in a vector of Texture objects (the value).
	AnimMap mAnimations;

	// Returns object represented as a std::string for debugging purposes.
	std::string toString(ClipsMap actionFrames);

private:
	// String filename to the data file for this sprite.
	std::string mFilename{};

	// load sprite data from files (called from constructor).
	virtual bool load();

	// Load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile(ClipsMap& actionsFrameCount);

	// Using previously loaded action names and sprite sheet clip coordinates, load in all sprite sheet animations for this sprite and store in an AnimMap (see typedef in Sprite.h).
	bool loadActionAnims();

	// Helper function to split some of the file parsing work into smaller chunks. Takes a string of 4 int values, comma delimited, and returns an bool success and an SDL_Rect as a tuple.
	std::tuple<bool, SDL_Rect> getRectFromCDV(std::string pCDV);

	// Helper function to ge the width and height of an SDL_Texture. Returned in a SDL_Point type (x=width, y=height).
	SDL_Point getSize(Texture text);
};