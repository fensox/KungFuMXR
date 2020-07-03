#pragma once

#include <SDL.h>

/* Global program constants. */
namespace FuGlobals {
	
	// Turn on all debug output
	constexpr bool DEBUG_MODE{ true };

	// FPS target for game loop as milliseconds per frame
	constexpr Uint32 FPS_TARGET{ 16 };
}