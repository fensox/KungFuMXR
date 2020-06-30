#include "SDLMan.h"
#include <SDL.h>
#include <iostream>
#include <string>

// Constructor. Takes title string to use as window caption if needed.
SDLMan::SDLMan(std::string windowCaption) {
	mWindowCaption = windowCaption;
}

// Destructor. Close down SDL.
SDLMan::~SDLMan() {
	SDL_Quit();
}

// Initilizes SDL, creates the window and renderer but does not show the window until a call to showWindow is made.
bool SDLMan::init() {
	// Attempt to initialize SDL returning false on failure and logging an error.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Failed in SDLMan constructor. SDL could not initialize! SDL Error: \n" << SDL_GetError();
		return false;
	}

	// Try to set texture filtering to linear. Warn if unable.
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) std::cerr << "Warning in SDLMan constructor. Linear Texture filtering not enabled!";
	
	// Create a window but don't show it. The calling program can do that through showWindow(bool)
	gWindow = SDL_CreateWindow(mWindowCaption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

}

// Show's the previously created window. Returns false on failure or false if SDL hasn't been initialized yet.
bool SDLMan::showWindow(bool win) {
	return true;
};