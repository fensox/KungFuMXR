#include <iostream>
#include "Sprite.h"

int main(int argc, char* argv[]) {
	bool success{ true };

	if (success) {
		Sprite sp = Sprite();
		
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