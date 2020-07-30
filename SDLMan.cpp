#include "SDLMan.h"
#include <SDL_image.h>
#include <iostream>
#include <vector>

// Constructor. Takes window caption string, bool to start with a fullscreen window, and width and height of starting window
// if we are not full screen. Width and height may be not used and will default to FuGlobals::VIEWPORT_WIDTH &
// FuGlobals::VIEWPORT_WIDTH.
SDLMan::SDLMan(std::string windowCaption, bool fullScreen, int width, int height) {
	mWindowCaption = windowCaption;
	mWindowW = width;
	mWindowH = height;
	mWindowFull = fullScreen;
}

// Destructor. Close down SDL.
SDLMan::~SDLMan() {
	if (FuGlobals::DEBUG_MODE) std::cerr << "Destructor: SDLMan" << std::endl;

	// Destroy buffer texture, renderer, and the window
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mRenderer = nullptr;
	mWindow = nullptr;

	// Close gamepad
	closeGamepad();

	// Free music
	Mix_FreeMusic(mMusic);
	mMusic = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
	Mix_Quit();
}

// Initilizes SDL, creates the window and renderer but does not show the window until a call to showWindow is made.
bool SDLMan::init() {
	// Attempt to initialize SDL returning false on failure and logging an error.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
		std::cerr << "Failed in SDLMan::init: SDL could not initialize. SDL Error: \n" << SDL_GetError();
		return false;
	}
	
	// Set up some window creation flags. Start window hidden (can be shown with a call to showWindow), borderless, and whethar full screen or not.
	int flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
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
	
	// Set resolution for renderer so SDL will keep our viewport aspect ratio when window size changes
	if (SDL_RenderSetLogicalSize(mRenderer, FuGlobals::VIEWPORT_WIDTH, FuGlobals::VIEWPORT_HEIGHT) != 0) {
		std::cerr << "Failed in SDLMan:init: Renderer could not be set to our viewport width/height. SDL Error: \n" << SDL_GetError();
		return false;
	}

	// Initialize renderer color for clearing the screen (using black)
	setDrawColor(0, 0, 0);

	// Initialize SDL_mixer for sound support
	if (Mix_OpenAudio(MIX_DEFAULT_FORMAT, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		std::cerr << "Failed in SDLMan::init, SDL_mixer could not initialize. SDL_mixer Error: \n" << Mix_GetError() << std::endl;;
		return false;
	}

	// Load in game controller mappings database.
	int iMapResult{ SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") };
	if (FuGlobals::DEBUG_MODE) std::cout << "SDLMan::openGamepad - Loaded gamecontrollerdb.txt mappings file with result: " << iMapResult << std::endl;
	if (iMapResult == -1) {
		std::cerr << "SDLMan::openGamepad - Warning: Unable to load controller mapping database! SDL Error: " << SDL_GetError() << std::endl;
	}

	// Attempt to open a game controller if one is connected. We disable controller events during gamepad opening to keep an SDL_CONTROLLERDEVICEADDED
	// event from being added to the event queue which would cause our openGamepad function to be called twice.
	SDL_GameControllerEventState(SDL_IGNORE);
	openGamepad();
	SDL_GameControllerEventState(SDL_ENABLE);

	// Initialize variables used for FPS calculation
	//memset(mFPSTimes, 0, sizeof(mFPSTimes)); //***DEBUG*** delete this if our new for loop works
	for (int i{}; i < FPS_AVG; ++i) mFPSTimes[i] = FPS_INIT;
	mFPSCount = 0;
	mFPS = FPS_INIT;
	mFPSLast = SDL_GetTicks();

	return true;
}

// Open a gamepad for use. Prints a warning to the error output stream if cannot open a controller but does not prevent game from continuing.
void SDLMan::openGamepad() {
	// make sure there is a gamepad connected and open the first one
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			mController1 = SDL_GameControllerOpen(i);
			mControllerID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(mController1));			
			if (mController1) {
				if (FuGlobals::DEBUG_MODE) {
					std::cout << "SDLMan::openGamepad - Game controller ID" << mControllerID << " opened." << std::endl;
					std::cout << "SDLMan::openGamepad - Using controller map: " << SDL_GameControllerMapping(mController1) << std::endl;
				}
			} else {
				std::cerr << "SDLMan::openGamepad: Warning: Unable to open game controller! SDL Error: " << SDL_GetError() << std::endl;
			}
		}
	}
}

// Returns the id of the current gamepad or a negative number if a gamepad is not opened.
int SDLMan::getGamepadID() {
	return mControllerID;
}

// Close the gamepad device we may have opened.
void SDLMan::closeGamepad() {
	SDL_GameControllerClose(mController1);
	mController1 = nullptr;
	if (FuGlobals::DEBUG_MODE) std::cout << "SDLMan::closeGamepad - Game controller ID" << mControllerID << " closed." << std::endl;
	mControllerID = -1;
}

// Renders to the screen the contents of the buffer
void SDLMan::refresh() {
	// Flip SDL's internal buffer to the window and clear buffer
	SDL_RenderPresent(mRenderer);
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

// Returns the height and width of a Texture object's wrapped SDL_Texture. Return type holding width/height is an SDL_Point.
SDL_Point SDLMan::getSize(std::shared_ptr<Texture> text) {
	SDL_Point size{};
	SDL_QueryTexture(text->getTexture(), NULL, NULL, &size.x, &size.y);
	return size;
}

// Set's the current draw color of the renderer with RGB values between 0 and 255. Optional alpha transparency value defaults to 255 opaque.
void SDLMan::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
}

// Draw's a line on screen with given starting and ending points using currently set draw color.
void SDLMan::drawLine(int x1, int y1, int x2, int y2) {
	SDL_RenderDrawLine(mRenderer, x1, y1, x2, y2);
}

// Draw's a line on screen with given starting and ending points using currently set draw color. Casts floating point parameter values to integers for screen drawing.
void SDLMan::drawLine(decimal x1, decimal y1, decimal x2, decimal y2) {
	SDL_RenderDrawLine(	mRenderer,
						static_cast<int>(x1),
						static_cast<int>(y1),
						static_cast<int>(x2),
						static_cast<int>(y2) );
}

// Draw's a rectangle outline with given coordinates and size.
void SDLMan::drawRect(int x, int y, int w, int h) {
	const SDL_Rect rect{ x, y, w, h };
	SDL_RenderDrawRect(mRenderer, &rect);
}

// Draw's a rectangle outline with given SDL_Rect.
void SDLMan::drawRect(const SDL_Rect& rect) {
	SDL_RenderDrawRect(mRenderer, &rect);
}

// Draw's a circle using the Midpoint Circle Algorithm found on Stack Exchange.
void SDLMan::drawCircle(int centerX, int centerY, int radius) {
	const int diameter{ radius * 2 };

	int x{ radius - 1 };
	int y{ 0 };
	int tx{ 1 };
	int ty{ 1 };
	int error{ tx - diameter };

	while (x >= y) {
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(mRenderer, centerX + x, centerY - y);
		SDL_RenderDrawPoint(mRenderer, centerX + x, centerY + y);
		SDL_RenderDrawPoint(mRenderer, centerX - x, centerY - y);
		SDL_RenderDrawPoint(mRenderer, centerX - x, centerY + y);
		SDL_RenderDrawPoint(mRenderer, centerX + y, centerY - x);
		SDL_RenderDrawPoint(mRenderer, centerX + y, centerY + x);
		SDL_RenderDrawPoint(mRenderer, centerX - y, centerY - x);
		SDL_RenderDrawPoint(mRenderer, centerX - y, centerY + x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

// Draw's a filled circle using the Midpoint Circle Algorithm found on Stack Exchange. Inneficient 'for' loop of smaller circles fills it in.
void SDLMan::drawCircleFilled(int centerX, int centerY, int radius) {
	for (int i{ 1 }; i < (radius + 1); ++i) drawCircle(centerX, centerY, i);
}

// Load in a Texture object using the passed in filename, transparancy color, and transparancy on/off switch. Returns a smart pointer to a Texture object.
std::unique_ptr<Texture> SDLMan::loadImage(std::string fileName, SDL_Color color, bool useTrans) {
	//Pointer to the SDL texture object we will wrap with Texture class and return
	SDL_Texture* newTexture {nullptr};

	// Load image at specified path
	SDL_Surface* loadedSurface{ IMG_Load(fileName.c_str()) };
	if (!loadedSurface) {
		std::cerr << "Failed in SDLMan::loadImage trying to perform IMG_Load on: \"" << fileName << "\"\nSDL_Image Error: " << IMG_GetError() << std::endl;
		return nullptr;
	}

	// Set the transparency color if the useTrans switch is true
	if (useTrans) SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, color.r, color.g, color.b));
		
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
	loadedSurface = nullptr;
	
	// Wrap the SDL_Texture in our Texture class and return as a smart pointer
	return std::make_unique<Texture>(newTexture);
}

// Load in a Texture object using the passed in filename. Transparancy information is attempted to be read automatically from file. Returns a smart pointer to a Texture object.
std::unique_ptr<Texture> SDLMan::loadImage(std::string fileName) {
	SDL_Color c{};
	return loadImage(fileName, c, false);
}

// Provide a pointer to the renderer for others to use to draw themselves.
SDL_Renderer* SDLMan::getRenderer() {
	return mRenderer;
}

// Load in a music file. Does not play immediately. Use playMusic(bool) to start and stop loaded music.
bool SDLMan::loadMusic(std::string musicFile) {
	bool success{ true };

	mMusic = Mix_LoadMUS(musicFile.c_str());
	if (!mMusic) {
		success = false;
		std::cerr << "Failed in SDLMan::loadMusic. SDL_mixer Error: \n" << Mix_GetError() << std::endl;
	}

	return success;
}

// Play the music file that is loaded in or resume it if it is paused. If it is already playing, pause it.
void SDLMan::toggleMusic() {
	//If there is no music playing
	if (!Mix_PlayingMusic()) {
		Mix_PlayMusic(mMusic, -1);
	} else {
		//If the music is paused
		if (Mix_PausedMusic()) {
			Mix_ResumeMusic();
		} else {
			Mix_PauseMusic();
		}
	}
}

// Calculates the current FPS using an averaging method of the last ten frames. Must be called every game tick for other FPS functions to work correctly.
void SDLMan::calculateFPS() {
	// fpsIndex is the position in the FPS averaging array. It ranges from 0 to FPS_AVG and rotates back to 0 after reaching FPS_AVG.
	Uint32 fpsIndex{ mFPSCount % FPS_AVG };

	// store the current time
	Uint32 getticks{ SDL_GetTicks() };

	// save the frame time value
	mFPSTimes[fpsIndex] = getticks - mFPSLast;

	// save the last frame time for the next fpsthink
	mFPSLast = getticks;

	// increment the frame count
	++mFPSCount;

	// test to see if the whole array has been written to or not. Stops strange values first FPS_AVG frames
	Uint32 count{};
	if (mFPSCount < FPS_AVG) {
		count = mFPSCount;
	} else {
		count = FPS_AVG;
	}

	// add up all the values and divide to get the average frame time.
	mFPS = 0;
	for (Uint32 i = 0; i < count; i++) mFPS += mFPSTimes[i];
	mFPS /= count;

	// now to make it an actual frames per second value
	mFPS = 1000.0 / mFPS;
}

// Returns the current average FPS count.
decimal SDLMan::getFPS() {
	// if this is the first five seconds of SDL running, return the FPS_INIT value to give our averaging time to catch up.
	if (SDL_GetTicks() > 5000) 	return mFPS;
	else return FPS_INIT;
}

// Outputs the FPS count to console.
void SDLMan::outputFPS() {
	std::cout << getFPS() << "\n";
}