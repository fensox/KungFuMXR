#include "PointF.h"

// Constructor takes two floating point values to set the coordinates.
PointF::PointF(decimal pX, decimal pY) {
	x = pX;
	y = pY;
}

// Constructor takes an int and a floating point value to set the coordinates of x and y respectively.
PointF::PointF(int pX, decimal pY) {
	x = static_cast<decimal>(pX);
	y = pY;
}

// Constructor takes a floating point and an int value to set the coordinates of x and y respectively.
PointF::PointF(decimal pX, int pY) {
	x = pX;
	y = static_cast<decimal>(pY);
}

// Returns an SDL_Point by static casting floating point coordinates to integers
SDL_Point PointF::getSDL_Point() {
	return SDL_Point{ static_cast<int>(x), static_cast<int>(y) };
}