#pragma once

#include <SDL.h>

// Global namespace typedef for floating point numbers. Adjusting this type here changes entire use of floating point types program wide.
typedef double decimal;

// Global program constants. All constants are in the FuGlobals namespace.
namespace FuGlobals {
	
	// Turn on all debug output
	constexpr bool DEBUG_MODE{ true };

	// Turn on music
	constexpr bool MUSIC{ false };

	// Turn on FPS readout
	constexpr bool SHOW_FPS{ false };

	// FPS target for game loop as milliseconds per frame (0 for unlimited).
	constexpr Uint32 FPS_TARGET{ 0 };

	// Viewport resolution. Game is scaled to fit various window sizes/resolutions but viewport is always this size internally. Needs to stay same aspect ratio ad WINDOW_ to look right.
	constexpr int VIEWPORT_WIDTH{ 1280 };
	constexpr int VIEWPORT_HEIGHT{ 720 };
	//constexpr int VIEWPORT_WIDTH{ 256 }; // NES Resolution
	//constexpr int VIEWPORT_HEIGHT{ 240 };

	// Default aspect ratio pre-calc'd for performance
	constexpr decimal ASPECT_RATIO{ static_cast<double>(VIEWPORT_WIDTH) / static_cast<double>(VIEWPORT_HEIGHT) };

	// Gravity multiplier - increased downward speed by this amount of pixels every GRAVITY_TIME
	constexpr decimal GRAVITY{ 1.5 };

	// This is a value in milliseconds to let pass each time before applying our GRAVITY velocity modyfier when object is falling
	constexpr int GRAVITY_TIME{ 250 };

	// This is the friction surfaces have that slows a players speed
	constexpr decimal FRICTION{ 1.5 };

	// Distance in pixels a player can get to the edge of the viewport when level boundry has been reached.
	constexpr int LEVEL_BOUNDS = 1;

	//***DEBUG*** Prob not needed as we don't really need to hold a rect as a float?
	// Rectangle struct to hold a rectangle in 2D space with floating point precision
	//struct Rect { decimal x{}; decimal y{}; decimal w{}; decimal h{}; };
}