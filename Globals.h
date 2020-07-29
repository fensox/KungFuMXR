#pragma once

#include <SDL.h>

// Global namespace typedef for floating point numbers. All floating point variables in the program use this typedef.
typedef double decimal;

// Global program constants. All constants are in the FuGlobals namespace.
namespace FuGlobals {
	
	constexpr bool		DEBUG_MODE				{ true };				// Turn on all debug output
	constexpr bool		MUSIC					{ false };				// Turn on music
	constexpr bool		SHOW_FPS				{ false };				// Turn on FPS readout
	constexpr Uint32	FPS_TARGET				{ 0 };					// FPS target for game loop as milliseconds per frame (0 for unlimited). i.e. for 60fps set to 16.
	constexpr int		VIEWPORT_WIDTH			{ 1280 };				// Internal rendering width - independant of window size. Scaled to fit whatever window needed.
	constexpr int		VIEWPORT_HEIGHT			{ 720 };				// Internal rendering height - independant of window size. Scaled to fit whatever window needed.
	
	constexpr int		JOYSTICK_DEAD_ZONE		{ 8000 };				// Analog joystick dead zone
	constexpr decimal	GRAVITY					{ 3 };					// Amount of pixels per real world second to drop player
	constexpr decimal	TERMINAL_VELOCITY		{ 10 };					// Maximum velocity that gravity can make a player fall
	constexpr decimal	GROUND_FRICTION			{ 1.4 };				// Friction that surfaces have that slows a sprites horizontal velocity when standing on that surface. Not applied when walking.
	constexpr decimal	AIR_FRICTION			{ 1.4 };				// Friction that air has that slows a sprites horizontal velocity when traveling through air and not standing
	constexpr Uint32	FRICTION_TIME			{ 100 };				// Milliseconds to let pass before applying friction that slows a sprites horizontal velocity
	constexpr int		LEVEL_BOUNDS			{ 1 };					// Distance in pixels a player can get to the edge of the viewport when level boundry has been reached.

}