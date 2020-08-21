#include "GameLoop.h"
#include "SDLMan.h"
#include "FuGlobals.h"
#include <iostream>

// Destructor
GameLoop::~GameLoop() {
	if (FuGlobals::DEBUG_MODE) std::cerr << "Destructor: GameLoop" << std::endl;

	mPlayer.reset();
	mSprites.reset();
	mLevel.reset();
	mSDL.reset();
}

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
	
	// initialize our Sprite holding vector. Holds all sprites but the player.
	mSprites = std::make_unique<std::vector<Sprite>>();

	// Load the player
	mPlayer = std::make_unique<MisterX>(mSDL);
	if (!mPlayer->load()) success = false;

	//***DEBUG***
	// This needs to be replaced with loading game level information from a game metafile not hardcoded like this
	// Load the first level
	if (!loadLevel("data/level1.dat")) success = false;

	// Return successful loading of game data.
	return (mSprites && success);
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
	// of this loop by setting the integer variable FPS_TARGET in the FuGlobals.h header.
	while (!quit) {
		// some time calculations to manage game loop speed
		double current = SDL_GetTicks();
		double elapsed = current - previous;
		previous = current;
		lag += elapsed;

		//***DEBUG***
		if (FuGlobals::MUSIC) mSDL->toggleMusic();

		// progress game logic without rendering to backbuffer until FPS target has been reached (for slow systems or network connections)
		while (lag >= FuGlobals::FPS_TARGET) {
			// handle input events
			quit = handleEvents();

			// process movements of sprites
			mPlayer->move(); // ***DEBUG***this could prob be stuffed inside the sprites vector..version 2 maybe
			for (int i{ 0 }; i < mSprites->size(); ++i) mSprites->at(i).move();
			
			// update our time lag calculations
			lag -= FuGlobals::FPS_TARGET;

			//***DEBUG***
			if (FuGlobals::SHOW_FPS) mSDL->outputFPS();

			// have our SDL wrapper update it's FPS calculations once per frame so our physics works correctly
			mSDL->calculateFPS();

			// if FPS target is set to 0 break out as we are going for as many frames as we can
			if (FuGlobals::FPS_TARGET == 0) break;
		}

		// render to back buffer
		mLevel->render();
		mPlayer->render();
		for (int i{ 0 }; i < mSprites->size(); ++i) mSprites->at(i).render();

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
		switch (e.type) {
			case SDL_QUIT:																				// Handle request to quit
				quit = true;
				break;
			case SDL_CONTROLLERDEVICEADDED:																// Handle a controller being connected
				mSDL->openGamepad();
				break;

			case SDL_CONTROLLERDEVICEREMOVED:															// Handle a controller being removed
				mSDL->closeGamepad();
				break;

			case SDL_CONTROLLERAXISMOTION:																// Handle gamepad analog sticks
				if (e.jaxis.which == mSDL->getGamepadID()) mPlayer->handleInputAnalogStick(e.caxis);
				break;

			case SDL_CONTROLLERBUTTONDOWN:																// Handle gamepad buttons
				if (e.jaxis.which == mSDL->getGamepadID()) mPlayer->handleInputGamepad(e.cbutton, true);
				break;

			case SDL_CONTROLLERBUTTONUP:																// Handle gamepad buttons
				if (e.jaxis.which == mSDL->getGamepadID()) mPlayer->handleInputGamepad(e.cbutton, false);
				break;

			case SDL_KEYDOWN:																			// Handle keyboard key down
				mPlayer->handleInputKeyboard(e.key.keysym.sym, true);
				break;

			case SDL_KEYUP:																				// Handle keyboard key released
				mPlayer->handleInputKeyboard(e.key.keysym.sym, false);
				break;
		}
    }
	
    return quit;
}