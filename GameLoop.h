#pragma once

#include "MisterX.h"
#include "Level.h"
#include <memory>

/* Runs the main game loop. This is the owner of various shared_ptr's including the current level,
 * the player object, SDLMan object, etc. GameLoop doles out weak_ptr's of these objects for other
 * game objects to use so thus acts as a communications hub for game objects to get information on each other.
 */
class GameLoop {

public:
	~GameLoop();

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
	// The SDLMan class that wraps most SDL functionality for us. Held here as a shared_ptr and held by Sprites and Level objects as a weak_ptr.
	std::shared_ptr<SDLMan> mSDL{ nullptr };

	// The current game level the player is on. Held here as a shared_ptr and held by Sprite objects as a weak_ptr.
	std::shared_ptr<Level> mLevel{ nullptr };

	// The Sprite for the player. Held here as a shared_ptr and held by Level objects as a weak_ptr.
	std::shared_ptr<MisterX> mPlayer{ nullptr };
};