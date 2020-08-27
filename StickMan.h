#pragma once

#include "Sprite.h"
#include "SDLMan.h"
#include <memory>

class StickMan : public Sprite {

public:
	StickMan(std::weak_ptr<SDLMan> mSDL);
};

