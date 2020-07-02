#pragma once

#include <SDL.h>
#include <string>
#include "Texture.h"
#include <memory>

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

	// Draws the screen
	void refresh();

	// Load in a Texture object using the passed in filename and SDL_Image functions. Returns a smart pointer to a Texture object.
	std::unique_ptr<Texture> loadImage(std::string fileName, SDL_Color color);

	// Provide a reference to our renderer for others to use to draw themselves.
	SDL_Renderer& getRenderer();

private:
	// Store's the caption for the window.
	std::string mWindowCaption{};

	// Store's the window width and height with defaults.
	//int mWindowW{ 1024 }, mWindowH{ 576 };
	int mWindowW{ 256 }, mWindowH{ 240 }; // NES Resolution

	// Full screen window or not
	bool mWindowFull{ false };

	// Pointer holding the SDL_Window we'll be rendering to
	SDL_Window* mWindow{ nullptr };

	// Pointer holding the SDL_Renderer for our window.
	SDL_Renderer* mRenderer{ nullptr };
};