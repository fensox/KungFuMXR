#pragma once

#include "FuGlobals.h"
#include <SDL.h>

// Represents a point in 2D screen space with floating point precision
class PointF {
public:
	// Constructor takes two floating point values to set the coordinates.
	PointF(decimal pX, decimal pY);

	// Constructor takes an int and a floating point value to set the coordinates of x and y respectively.
	PointF(int pX, decimal pY);

	// Constructor takes a floating point and an int value to set the coordinates of x and y respectively.
	PointF(decimal pX, int pY);

	// The coordinates in 2D screen space.
	decimal x{};
	decimal y{};

	// Returns an SDL_Point by static casting floating point coordinates to integers
	SDL_Point getSDL_Point();
};