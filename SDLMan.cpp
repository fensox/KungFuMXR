#include "SDLMan.h"
#include "Texture.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "Globals.h"

// Constructor. Takes title string to use as window caption if needed.
SDLMan::SDLMan(std::string windowCaption) {
	mWindowCaption = windowCaption;
}

// Destructor. Close down SDL.
SDLMan::~SDLMan() {
	//Destroy window	
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;
	mRenderer = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
}

// Initilizes SDL, creates the window and renderer but does not show the window until a call to showWindow is made.
bool SDLMan::init() {
	// Attempt to initialize SDL returning false on failure and logging an error.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Failed in SDLMan::init: SDL could not initialize. SDL Error: \n" << SDL_GetError();
		return false;
	}

	// Try to set texture filtering to linear. Warn if unable.
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) std::cerr << "Warning in SDLMan::init: Linear Texture filtering not enabled!" << std::endl;
	
	// Set up some window creation flags. Start window hidden (can be shown with a call to showWindow), borderless, and whethar full screen or not.
	int flags = SDL_WINDOW_HIDDEN;
	if (mWindowFull) flags = flags | SDL_WINDOW_FULLSCREEN;
	
	// Create a window
	mWindow = SDL_CreateWindow(mWindowCaption.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWindowW, mWindowH, flags);
	if (!mWindow) {
		std::cerr << "Failed in SDLMan::init: Window could not be created. SDL Error: \n" << SDL_GetError();
		return false;
	}
	
	// Create renderer for window (defaults to no vSync which can later be turned on with setVSync).
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!mRenderer) {
		std::cerr << "Failed in SDLMan:init: Renderer could not be created. SDL Error: \n" << SDL_GetError();
		return false;
	}
	
	// Initialize renderer color for clearing the screen
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	// Initialize variables used for FPS calculation
	memset(fpsTimes, 0, sizeof(fpsTimes));
	fpsCount = 0;
	fps = 0;
	fpsLast = SDL_GetTicks();

	return true;
}

// Draws the screen
void SDLMan::refresh() {
	// Flip backbuffer to display
	SDL_RenderPresent(mRenderer);
}

void SDLMan::clearBackBuffer() {
	SDL_RenderClear(mRenderer);
}

// Show or hide the window. Returns false if SDL hasn't been initialized yet.
bool SDLMan::showWindow(bool win) {
	// first check if the window is nullptr
	if (mWindow == nullptr) {
		std::cerr << "Failed in SDLMan::showWindow: Attempted to show a null SDL_Window." << std::endl;
		return false;
	}

	// toggle the window visability
	if (win) {
		SDL_ShowWindow(mWindow);
		SDL_RaiseWindow(mWindow);
	} else {
		SDL_HideWindow(mWindow);
	}	

	return true;
}

// Return's the window height.
int SDLMan::getWindowH() {
	return mWindowH;
}

// Set's the window height.
void SDLMan::setWindowH(int windowH){
	mWindowH = windowH;
}

// Return's the window width.
int SDLMan::getWindowW() {
	return mWindowW;
}

// Set's the window width.
void SDLMan::setWindowW(int windowW) {
	mWindowW = windowW;
}

// Set's the window fullscreen boolean value
void SDLMan::setFullscreen(bool fs) {
	mWindowFull = fs;

	if (mWindowFull) {
		SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);
	}
	else {
		SDL_SetWindowFullscreen(mWindow, 0);
	}
}

// Load in a Texture object using the passed in filename and SDL_Image functions. Returns a smart pointer to a Texture object.
std::unique_ptr<Texture> SDLMan::loadImage(std::string fileName, SDL_Color color) {
	//Pointer to the SDL texture object we will wrap with Texture class and return
	SDL_Texture* newTexture {nullptr};

	// Load image at specified path
	SDL_Surface* loadedSurface{ IMG_Load(fileName.c_str()) };
	if (!loadedSurface) {
		std::cerr << "Failed in SDLMan::loadImage trying to perform IMG_Load on: \"" << fileName << "\"\nSDL_Image Error: " << IMG_GetError() << std::endl;
		return nullptr;
	}

	// Set the transparency color
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, color.r, color.g, color.b));
		
	//Create texture from surface pixels
	newTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
	if (!newTexture)
	{
		std::cerr << "Failed in SDLMan::loadImage trying to create texture from surface on: \"" << fileName << "\"\n";
		std::cerr << "SDL_GetError(): " << SDL_GetError() << "\n";
		std::cerr << "IMG_GetError(): " << IMG_GetError() << std::endl;
		return nullptr;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);
	
	// Wrap the SDL_Texture in our Texture class and return as a smart pointer
	return std::make_unique<Texture>(newTexture);
}

// Provide a reference to the renderer for others to use to draw themselves.
SDL_Renderer& SDLMan::getRenderer() {
	return *mRenderer;
}

// Outputs the FPS count to console using an averaging method.
void SDLMan::outputFPS() {
	// fpsIndex is the position in the FPS averaging array. It ranges from 0 to 10 and rotates back to 0 after reaching 10.
	Uint32 fpsIndex{ fpsCount % 10 };
	
	// store the current time
	Uint32 getticks{ SDL_GetTicks() };

	// save the frame time value
	fpsTimes[fpsIndex] = getticks - fpsLast;

	// save the last frame time for the next fpsthink
	fpsLast = getticks;

	// increment the frame count
	++fpsCount;

	// test to see if the whole array has been written to or not. Stops strange values first 10 frames
	Uint32 count;
	if (fpsCount < 10) {
		count = fpsCount;
	} else {
		count = 10;
	}

	// add up all the values and divide to get the average frame time.
	fps = 0;
	for (Uint32 i = 0; i < count; i++) fps += fpsTimes[i];
	fps /= count;

	// now to make it an actual frames per second value
	fps = 1000.f / fps;

	// output
	std::cout << fps << "\n";
}