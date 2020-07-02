#include <iostream>
#include "Thomas.h"
#include "SDLMan.h"
#include "Globals.h"
#include "Sprite.h"

int main(int argc, char* argv[]) {
	bool success = true;
	
	// Initialize our SDL wrapper class ad a shared smart pointer to manage SDL things
	std::shared_ptr<SDLMan> sdl{ std::make_shared<SDLMan>("Kung Fu Mr. X's Revenge") };
	success = sdl->init();
	sdl->setFullscreen(false);

	// Load in all the sprites
	if (success) {
		// Load the player
		Thomas player = Thomas(sdl);
		
		sdl->showWindow(true);
		player.render();
		sdl->refresh();
		SDL_Delay(5000);

	}

	return 0;
}