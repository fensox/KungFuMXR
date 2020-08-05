#pragma once

#include <SDL.h>

// Global namespace typedef for floating point numbers. All floating point variables in the program use this typedef.
typedef double decimal;

// Global program constants. All constants are in the FuGlobals namespace.
namespace FuGlobals {
	
	constexpr bool		DEBUG_MODE				{ true };				// Turn on all debug output
	constexpr bool		MUSIC					{ false };				// Turn on music
	constexpr bool		SHOW_FPS				{ false };				// Turn on FPS readout
	constexpr Uint32	FPS_TARGET				{ 16 };					// FPS target for game loop as milliseconds per frame (0 for unlimited). i.e. for 60fps set to 16, 120fps set to 8, etc.
	constexpr int		VIEWPORT_WIDTH			{ 1280 };				// Internal rendering width - independant of window size. Scaled to fit whatever window needed.
	constexpr int		VIEWPORT_HEIGHT			{ 720 };				// Internal rendering height - independant of window size. Scaled to fit whatever window needed.
	
	constexpr int		JOYSTICK_DEAD_ZONE		{ 8000 };				// Analog joystick dead zone
	constexpr decimal	GRAVITY					{ 15 };					// Gravity defined as pixels/second of downward velocity to add to sprites
	constexpr decimal	TERMINAL_VELOCITY		{ 720 };				// Maximum pixels/second of downward velocity that gravity can make a player fall
	constexpr decimal	GROUND_FRICTION			{ 38.2 };				// Amount of horizontal pixels/second a solid surface slows a sprite. Not applied when sprite under powered motion.
	constexpr decimal	AIR_FRICTION			{ 38.2 };				// Amount of horizontal pixels/second the air slows a sprite when not standing on a solid surface. Not applied when sprite under powered motion.
	constexpr int		LEVEL_BOUNDS			{ 10 };					// Distance in pixels a player can get to the edge of the viewport when level boundry has been reached.

}