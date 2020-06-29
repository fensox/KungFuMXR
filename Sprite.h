#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include <tuple>

class Sprite
{
public:
	// constructor that takes in sprite data filename and calls load() to load the sprite in
	Sprite(std::string pFilename);

	// returns the current texture to be rendered as a pointer to an SDL_Texture
	SDL_Texture* getTextureToRender();

	// returns the current texture's collision rectangle as a pointer to an SDL_Rect
	SDL_Rect* getCollisionRect();

	// access function to get this sprite's position in 2D space
	SDL_Point& getPosition();

protected:
	// position of sprite
	SDL_Point mPosition{};
	
	// string filename to the data file for this sprite
	std::string mFilename{};

	// string holding the current action mode this sprite is in
	std::string mActionMode{};

	// int holding the current frame of animation we are in
	int mFrame{};

	// unordered map to hold key/value pairs of actions and their animations
	//std::unordered_map< std::string, std::vector<SDL_Texture> > mAnimations;

private:
	typedef std::unordered_map<std::string, std::vector<SDL_Rect>> ClipsMap;

	// load sprite data from files (typically called from constructor)
	virtual bool load();

	// load the initial data file in with action mode names and animation frame counts. Store in passed in map and return boolean success.
	bool loadDataFile(ClipsMap& actionsFrameCount);

	// Helper function to split some of the file parsing work into smaller chunks. Takes a string of 4 int values, comma delimited, and returns an bool success and an SDL_Rect as a tuple.
	std::tuple<bool, SDL_Rect> getRectFromCDV(std::string pCDV);
};