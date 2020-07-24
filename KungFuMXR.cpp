#include <memory>
#include <iostream>
#include "GameLoop.h"

/*
 * Entrypoint of the game.
 *
 * A few things to note for converting to different systems. In he Globals.h file is a typedef for
 * floating point numbers. It is using double by default and can be changed in this one spot for a
 * global change in type.
 * 
*/
int main(int argc, char* argv[]) {
	bool success = true;

	// Create a main game object.
	std::unique_ptr<GameLoop> game{ std::make_unique<GameLoop>() };

	// Call our function to initialize our graphics and sound systems
	if (!game->initGameSystems()) {
		success = false;
		std::cerr << "Failed in main. GameLoop::initGameSystems returned false." << std::endl;
	}

	// If we succeeded in initializing graphics systems now try to load in game data
	if (!(success && game->loadGameData())) {
		success = false;
		std::cerr << "Failed in main. GameLoop::loadGameData returned false." << std::endl;
	}

	// Start the game
	if (success) game->runGameLoop();

	return 0;
}