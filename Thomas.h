#pragma once
#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class Thomas : public Sprite
{
public:
	// Use base Sprite constructor
	Thomas(std::shared_ptr<SDLMan> sdlMan);

	// custom sprite move function
	void move();

protected:
	const std::string mMetaFilename{ "data/thomas.dat" };
	const std::string mSpriteSheet{ "data/thomas.png" };
	const SDL_Color mTrans{ 255, 0, 255, SDL_ALPHA_OPAQUE };
	const std::string mName{ "Thomas" };
};

