#include "Texture.h"
#include <SDL.h>

Texture::Texture(SDL_Texture* ptrText) {
	mText = ptrText;
};

Texture::~Texture() {
	SDL_DestroyTexture(mText);
	mText = nullptr;
};

SDL_Texture* Texture::getTexture() {
	return mText;
}