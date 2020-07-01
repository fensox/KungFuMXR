#include <iostream>
#include "Sprite.h"
#include "SDLMan.h"

int main(int argc, char* argv[]) {
	bool success = true;
	
	// Initialize our SDL wrapper class ad a shared smart pointer to manage SDL things
	std::shared_ptr<SDLMan> sdl{ std::make_shared<SDLMan>("Kung Fu Mr. X's Revenge") };
	success = sdl->init();
	sdl->setFullscreen(true);

	// Load in all the sprites
	if (success) {
		Sprite sp = Sprite(sdl);
		
		sdl->showWindow(true);
		sp.render();
		sdl->refresh();
		SDL_Delay(5000);


		/*
		//***DEBUG***
		SDL_Point tmpPoint = sp.getPosition();
		SDL_Rect tmpRect = sp.getCollisionRect();
		std::cout << "Sprite's position: " << tmpPoint.x << ", " << tmpPoint.y << "\n";
		std::cout << "Sprite's collision rectangle: " << tmpRect.x << ", " << tmpRect.y << ", " << tmpRect.w << ", " << tmpRect.h << std::endl;
		*/
	}

	return 0;
}