#pragma once

#include "Sprite.h"

/*	Player class to represent the player and derived from the generic Sprite class. 
	Extends Sprite to add custom movement for the player and animation control.
*/
class Player : public Sprite
{
public:		
	// adjusts sprite position using integer x and y velocity parameters
	void move(int velocityX, int velocityY);
};