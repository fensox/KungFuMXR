#include "Thomas.h"
#include "SDLMan.h"
#include <memory>

Thomas::Thomas(std::shared_ptr<SDLMan> sdlMan) : Sprite(sdlMan) {
	// set our Thomas specific info
	//mMetaFilename{ "data/thomas.dat" };
	//mSpriteSheet{ "data/thomas.png" };
	//mTrans{ 255, 0, 255, 0 };
	//mName{ "Sprite" };
};

void Thomas::move() {
	//do some moving
}