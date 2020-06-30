#pragma once

#include <SDL.h>

/* A very simple wrapper class for SDL_Texture. Facilitates storing textures in containers. */
class Texture
{
public:
	Texture(SDL_Texture* ptrText);

	~Texture();

	SDL_Texture* getTexture();

private:
	SDL_Texture* mText;
};

