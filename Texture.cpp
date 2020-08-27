#include "Texture.h"
#include "FuGlobals.h"
#include <iostream>

// Constructor takes a pointer to an SDL_Texture and stores some information about it for quick access later.
Texture::Texture(SDL_Texture* ptrText) {
	mText = ptrText;

	// Store size of texture. We are doing this assuming it is quicker to return a
	// local member variable holding the size than having to call SDL functions every time.
	if (ptrText != nullptr) {
		SDL_QueryTexture(mText, NULL, NULL, &mSize.x, &mSize.y);
	}
};

// Destructor destroys the texture properly.
Texture::~Texture() {
	if constexpr (FuGlobals::DEBUG_MODE) std::cerr << "Destructor: Texture" << std::endl;
	SDL_DestroyTexture(mText);
	mText = nullptr;
};

// Returns a pointer to the SDL_Texture we are wrapping.
SDL_Texture* Texture::getTexture() {
	return mText;
}

// Returns, by const reference for performance, the size of the texture in pixels stored in an SDL_Point
const SDL_Point& Texture::getSize() {
	return mSize;
}