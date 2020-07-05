#pragma once

#include "Thomas.h"
#include "Level.h"
#include <memory>

/* Runs the main game loop. */
class GameLoop
{
public:
	// Initializes the graphics and sound systems.
	bool initGameSystems();

	// Loads in game data.
	bool loadGameData();

	// Runs the main game loop.
	void runGameLoop();

	// Handles input events. Returns true on a quit game event.
	bool handleEvents();

	// Load in level data from file passed in by parameter. Filepath relative to executable. Return bool success.
	bool loadLevel(std::string lvlDataFile);

private:
	// The SDLMan class that wraps most SDL functionality for us.
	std::shared_ptr<SDLMan> mSDL{ nullptr };

	// The current game level the player is on.
	std::unique_ptr<Level> mLevel{ nullptr };

	// The Sprite for the player
	std::unique_ptr<Thomas> mPlayer{ nullptr };

	// Container to hold all non-player sprites in the game (smart pointer to a vector)
	std::unique_ptr<std::vector<Sprite>> sprites{ nullptr };
};