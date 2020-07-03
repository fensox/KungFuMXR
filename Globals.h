#pragma once

#include <SDL.h>

/* Global program constants. */
namespace FuGlobals {
	
	// Turn on all debug output
	constexpr bool DEBUG_MODE{ false };

	// FPS target for game loop as milliseconds per frame (0 for unlimited).
	constexpr Uint32 FPS_TARGET{ 0 };
}