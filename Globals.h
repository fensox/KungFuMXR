#pragma once

#include <SDL.h>

/* Global program constants. */
namespace FuGlobals {
	
	// Turn on all debug output
	constexpr bool DEBUG_MODE{ true };

	// Turn on FPS readout
	constexpr bool SHOW_FPS{ false };

	// FPS target for game loop as milliseconds per frame (0 for unlimited).
	constexpr Uint32 FPS_TARGET{ 0 };

	// Starting window size when not full screen
	constexpr int WINDOW_WIDTH{ 1280 };
	constexpr int WINDOW_HEIGHT{ 720 };
	
	// Viewport resolution. Game is scaled to fit various window sizes/resolutions but viewport is always this size internally.
	constexpr int VIEWPORT_WIDTH{ 1280 };
	constexpr int VIEWPORT_HEIGHT{ 720 };
	//constexpr int VIEWPORT_WIDTH{ 256 }; // NES Resolution
	//constexpr int VIEWPORT_HEIGHT{ 240 };

	// Gravity
	constexpr int GRAVITY{ 5 };

	// Distance in pixels a player can get to the edge of the viewport when level boundry has been reached.
	constexpr int LEVEL_BOUNDS = 15;
}