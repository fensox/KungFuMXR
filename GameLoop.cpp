#include <memory>
#include <iostream>
#include "GameLoop.h"
#include "SDLMan.h"
#include "Globals.h"

// Initializes the graphics and sound systems.
bool GameLoop::initGameSystems() {
	// Initialize our SDL wrapper class and a shared smart pointer to manage SDL things
	mSDL = std::make_shared<SDLMan>("Kung Fu Mr. X's Revenge");

	// Set some window parameters and initialize SDL if we can.
	mSDL->setFullscreen(false);
	return mSDL->init();;
}

// Loads in game data.
bool GameLoop::loadGameData() {
	// initialize our Sprite holding vector. Holds all non-player sprites.
	sprites = std::make_unique<std::vector<Sprite>>();

	// Load the player
	player = std::make_unique<Thomas>(mSDL);

	// Return successful loading of game data.
	return (sprites && player);
}

// Runs the main game loop.
void GameLoop::runGameLoop() {
	// Show the window
	mSDL->showWindow(true);
	
	// Set some variables for use in our loop
	bool quit{ false };					// whethar to quit the loop or not
	double previous = SDL_GetTicks();	// holds current time in milliseconds since SDL init
	double lag = 0;						// will hold lag between game time elapsed and real time elapsed
	
	// Start the main loop
	while (!quit) {
		// some time calculations to manage game loop speed
		double current = SDL_GetTicks();
		double elapsed = current - previous;
		previous = current;
		lag += elapsed;

		// clear back buffer for our new frame
		mSDL->clearBackBuffer();

		// progress game logic without rendering to backbuffer until FPS target has been reached
		while (lag >= FuGlobals::FPS_TARGET) {
			// handle input events
			quit = handleEvents();

			// update our time lag calculations
			lag -= FuGlobals::FPS_TARGET;

			//***DEBUG***
			if (FuGlobals::DEBUG_MODE) mSDL->outputFPS();
		}

		// render to back buffer
		player->render();

		// update the screen
		mSDL->refresh();
	}
}

// Handles input events. Returns true on a quit game event.
bool GameLoop::handleEvents() {
    SDL_Event e;
    bool quit{ false };

    // Cycle through all events on the event queue
    while (SDL_PollEvent(&e)) {
		// Set quit boolean when user requests it
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN) {
			// Handle keyboard presses
            player->move(e.key.keysym.sym);
        }
    }

    return quit;
}