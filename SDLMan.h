#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include "Texture.h"
#include <memory>
#include "Globals.h"

/* SDLMan - SDL Manager Utility Class
 *
 * Helper class to manage all sorts of SDL goings on.
 * Wraps a SDL_Window and SDL_Renderer. Supplies functions
 * to draw things, a destructor to close down SDL objects, etc.
 *
 * Constructor initializes SDL but no window is opened or renderer
 * obtained until a call to openWindow() is made.
 */
class SDLMan {

public:
	// Constructor. Takes title string to use as window caption if needed.
	SDLMan(std::string windowCaption);

	// Destructor
	~SDLMan();

	// Initilizes SDL, creates the window and renderer but does not show the window until a call to showWindow is made.
	bool init();

	// Show's the previously created window. Returns false on failure or false if SDL hasn't been initialized yet.
	bool showWindow(bool win);

	// Return's the window height.
	int getWindowH();

	// Set's the window height.
	void setWindowH(int windowH);

	// Return's the window width.
	int getWindowW();

	// Set's the window width.
	void setWindowW(int windowW);

	// Set's the window fullscreen boolean value
	void setFullscreen(bool fs);

	// Tells SDLMan to poll SDL for the window size and update our window size variables
	void updateWindowSize();

	// Draws the screen
	void refresh();

	// Clears the drawing buffer so we have a clean area to draw on.
	void clearBackBuffer();

	// Returns the height and width of a texture as an SDL_Point.
	SDL_Point getSize(std::shared_ptr<Texture> text);

	// Load in a music file. Does not play immediately. Use playMusic(bool) to start and stop loaded music.
	bool loadMusic(std::string musicFile);

	// Play the music file that is loaded in or resume it if it is paused. If it is already playing, pause it.
	void toggleMusic();

	// Load in a Texture object using the passed in filename, transparancy color, and transparancy on/off switch. Returns a smart pointer to a Texture object.
	std::unique_ptr<Texture> loadImage(std::string fileName, SDL_Color color, bool useTrans);

	// Provide a pointer to our renderer for others to use to draw themselves.
	SDL_Renderer* getRenderer();

	// Outputs the FPS count to console using an averaging method.
	void outputFPS();

private:
	// Holds the music that will be played in the background
	Mix_Music* gMusic{ nullptr };

	// Pointer holding the SDL_Window we'll be rendering to
	SDL_Window* mWindow{ nullptr };

	// Pointer holding the SDL_Renderer for our window.
	SDL_Renderer* mRenderer{ nullptr };

	// Store's the caption for the window.
	std::string mWindowCaption{};

	// Store's the window width and height with defaults.
	int mWindowW{ FuGlobals::WINDOW_WIDTH }, mWindowH{ FuGlobals::WINDOW_HEIGHT };

	// Full screen window or not
	bool mWindowFull{ false };

	// An array to store frame times for averaging FPS to smooth out calculation
	Uint32 fpsTimes[10];

	// Last calculated SDL_GetTicks
	Uint32 fpsLast;

	// total frames rendered
	Uint32 fpsCount;

	// the value you want
	float fps;
};