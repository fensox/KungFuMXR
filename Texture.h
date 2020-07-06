#pragma once

#include <SDL.h>

/* A very simple wrapper class for SDL_Texture. Facilitates storing textures in containers. */
class Texture
{
public:
	// Constructor takes a pointer to an SDL_Texture.
	Texture(SDL_Texture* ptrText);

	// Destructor destroys the texture properly.
	~Texture();

	// Returns a pointer to the SDL_Texture we are wrapping.
	SDL_Texture* getTexture();

	// Returns the size of the texture in pixels stored in an SDL_Point.
	const SDL_Point& getSize();

private:
	// A pointer to our texture we are wrapping.
	SDL_Texture* mText;

	// The width and height of our texture stored in an SDL_Point
	SDL_Point mSize{};
};

