#include "StickMan.h"
#include <iostream>

// constructor
StickMan::StickMan(std::weak_ptr<SDLMan> mSDL) : Sprite{ mSDL } {
	// set our Stick Man specific members
	mMetaFilename = "data/StickMan.dat";
	mSpriteSheet = "data/MasterSS.png";
	mStartingActionMode = "WALK_RIGHT";
	mTrans = SDL_Color{ 255, 0, 255, 0 };
	mName = "StickMan";
	mScale = 3;

	// set default action mode set for this sprite into our action mode member
	setActionMode( mStartingActionMode, true );

	// load sound effects
	//mSDL.lock()->addSoundEffect("MRX_PUNCH", "data/mrx_punch.wav");
	//mSDL.lock()->addSoundEffect("MRX_KICK", "data/mrx_kick.wav");
}