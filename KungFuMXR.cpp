#include <memory>
#include "GameLoop.h"

int main(int argc, char* argv[]) {
	bool success = true;

	// Create a main game object.
	std::unique_ptr<GameLoop> game{ std::make_unique<GameLoop>() };

	// Call our function to initialize our graphics and sound systems
	success = game->initGameSystems();

	// If we succeeded in initializing graphics systems then load in game data
	if (success) success = game->loadGameData();

	// Start the game
	if (success) game->runGameLoop();

	return 0;
}