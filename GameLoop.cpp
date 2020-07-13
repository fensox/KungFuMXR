#include <iostream>
#include "GameLoop.h"
#include "SDLMan.h"
#include "Globals.h"

// Initializes the graphics and sound systems.
bool GameLoop::initGameSystems() {
	// Initialize our SDL wrapper class and a shared smart pointer to manage SDL things
	mSDL = std::make_shared<SDLMan>("Kung Fu Mr. X's Revenge");

	// Try to have SDLMan to initialize all systems
	return mSDL->init();;
}

// Loads in game data.
bool GameLoop::loadGameData() {
	bool success{ true };
	
	// initialize our Sprite holding vector. Holds all non-player sprites.
	sprites = std::make_unique<std::vector<Sprite>>();

	// Load the player
	mPlayer = std::make_unique<Thomas>(mSDL);
	if (!mPlayer->load()) success = false;

	//***DEBUG***
	// This needs to be replaced with loading game level information from a game metafile not hardcoded like this
	// Load the first level
	if (!loadLevel("data/level1.dat")) success = false;

	// Return successful loading of game data.
	return (sprites && success);
}

// Load in the current level
bool GameLoop::loadLevel(std::string lvlDataFile) {
	bool success{ true };

	// Load the requested level and give the player object it's start position and a smart pointer to our new level
	mLevel = std::make_shared<Level>(lvlDataFile, mSDL);
	if (!mLevel->load()) {
		success = false;
		std::cerr << "Failed in GameLoop::loadLevel. Level::load returned false." << std::endl;
	} else {
		// set player starting position as given by level and give the player and level smart pointers to each other
		if (mPlayer) {
			mPlayer->setX(mLevel->getPlayStart().x);
			mPlayer->setY(mLevel->getPlayStart().y);
			mPlayer->setLevel(mLevel);
			mLevel->setFollowSprite(mPlayer);
		} else {
			success = false;
		}
	}

	return success;
}

// Runs the main game loop.
void GameLoop::runGameLoop() {
	// Show the window
	mSDL->showWindow(true);

	// Set some variables for use in our loop
	bool quit{ false };					// whethar to quit the loop or not
	double previous = SDL_GetTicks();	// holds current time in milliseconds since SDL init - game loop speed/FPS management
	double lag = 0;						// will hold lag between game time elapsed and real time elapsed - game loop speed/FPS management
	
	// Start the main loop
	// Game loop uses "Fixed update time step, variable rendering" method written about
	// in the book Game Programming Patterns by Robert Nystrom. Adjust performance
	// of this loop by setting the integer variable in the Globals.h file.
	while (!quit) {
		// some time calculations to manage game loop speed
		double current = SDL_GetTicks();
		double elapsed = current - previous;
		previous = current;
		lag += elapsed;

		//***DEBUG***
		if (FuGlobals::MUSIC) mSDL->toggleMusic();

		// progress game logic without rendering to backbuffer until FPS target has been reached
		while (lag >= FuGlobals::FPS_TARGET) {
			// handle input events
			quit = handleEvents();

			// process movements of sprites. For the player this is a physics movement (movement to player not initiated by player!)
			mPlayer->move();
			for (int i{ 0 }; i < sprites->size(); ++i) sprites->at(i).move();
			
			// update our time lag calculations
			lag -= FuGlobals::FPS_TARGET;

			//***DEBUG***
			if (FuGlobals::SHOW_FPS) mSDL->outputFPS();

			// if FPS target is set to 0 break out as we are going for as many frames as we can
			if (FuGlobals::FPS_TARGET == 0) break;
		}

		// render to back buffer
		mLevel->render();
		mPlayer->render();

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
        if (e.type == SDL_QUIT) {																	// Handle request to quit
			quit = true;
        } else if (e.type == SDL_KEYDOWN) {															// Handle keyboard presses
            mPlayer->playerInput(e.key.keysym.sym);
		}
    }

    return quit;
}