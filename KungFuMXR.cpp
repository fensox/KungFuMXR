#include <iostream>
#include "Sprite.h"

int main(int argc, char* argv[]) {
	bool success{ true };

	
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	if (success) {
		Sprite sp = Sprite("Data/Thomas.dat");
		
		/*
		//***DEBUG***
		SDL_Point tmpPoint = sp.getPosition();
		SDL_Rect tmpRect = sp.getCollisionRect();
		std::cout << "Sprite's position: " << tmpPoint.x << ", " << tmpPoint.y << "\n";
		std::cout << "Sprite's collision rectangle: " << tmpRect.x << ", " << tmpRect.y << ", " << tmpRect.w << ", " << tmpRect.h << std::endl;
		*/
	}

	SDL_Quit();

	return 0;
}