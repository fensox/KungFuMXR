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
 * Wraps an SDL_Window and SDL_Renderer. Supplies functions
 * to assist in drawing things, a destructor to close down
 * SDL objects, functions for loading images, etc.
 *
 * The FuGlobals namespace with the VIEWPORT_HEIGHT and
 * VIEWPORT_WIDTH is required and SDL's renderer is set to
 * those dimensions so it can properly scale on window changes.
 *
 * Constructor initializes SDL but no window is opened or renderer
 * obtained until a call to openWindow() is made. Constructor takes
 * a caption argument for the window and a width/height for the
 * back buffer texture. This buffer size will stay constant despite
 * the window size and be scaled to fit window when render() is called.
 */
class SDLMan {

public:
	// Constructor. Takes window caption string, bool to start with a fullscreen window, and width and height of starting window
	// if we are not full screen. Width and height may be not used and will default to FuGlobals::VIEWPORT_WIDTH &
	// FuGlobals::VIEWPORT_WIDTH.
	SDLMan(std::string windowCaption, bool fullScreen = false, int width = FuGlobals::VIEWPORT_WIDTH, int height = FuGlobals::VIEWPORT_HEIGHT);

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

	// Draws the buffer to the screen and clears the buffer
	void refresh();

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
	Mix_Music* mMusic{ nullptr };

	// Pointer holding the SDL_Window we'll be rendering to
	SDL_Window* mWindow{ nullptr };

	// Pointer holding the SDL_Renderer for our window.
	SDL_Renderer* mRenderer{ nullptr };

	// Smart pointer to a Texture objext holding the SDL_Texture used as our backbuffer drawing surface.
	std::unique_ptr<Texture> mBuffer{ nullptr };

	// Store's the caption for the window.
	std::string mWindowCaption{};

	// Store's the window width and height with defaults.
	int mWindowW{ FuGlobals::VIEWPORT_WIDTH };
	int mWindowH{ FuGlobals::VIEWPORT_HEIGHT };

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