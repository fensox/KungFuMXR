#include "SDLMan.h"
#include <SDL_image.h>
#include <iostream>
#include <vector>

// Constructor. Takes window caption string and width and height of backbuffer to create or uses defaults if dimensions not specified.
SDLMan::SDLMan(std::string windowCaption, int width = 1280 , int height = 720 ) {
	mWindowCaption = windowCaption;
	mWindowW = width;
	mWindowH = height;
	mBufferW = width;
	mBufferH = height;
}

// Destructor. Close down SDL.
SDLMan::~SDLMan() {
	// Destroy buffer texture, renderer, and the window
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mRenderer = nullptr;
	mWindow = nullptr;

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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cerr << "Failed in SDLMan::init: SDL could not initialize. SDL Error: \n" << SDL_GetError();
		return false;
	}

	//***DEBUG*** Turned off texture filtering as transparency artifacts were halo'd around each sprite (what is linear texture filtering anyhow...no time to google)
	// Try to set texture filtering to linear. Warn if unable.
	//if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) std::cerr << "Warning in SDLMan::init: Linear Texture filtering not enabled!" << std::endl;
	
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
	
	// Initialize renderer color for clearing the screen (using black)
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	// Create drawing buffer if triple buffering const enabled
	if (TRIPLE_BUFFERING) {
		if (!createBuffer()) {
			std::cerr << "Failed in SDLMan:init: Buffer could not be created. SDL Error: \n" << SDL_GetError();
			return false;
		}
	}

	// Initialize SDL_mixer for sound support
	if (Mix_OpenAudio(MIX_DEFAULT_FORMAT, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		std::cout << "Failed in SDLMan::init, SDL_mixer could not initialize. SDL_mixer Error: \n" << Mix_GetError() << std::endl;;
		return false;
	}

	// Initialize variables used for FPS calculation
	memset(fpsTimes, 0, sizeof(fpsTimes));
	fpsCount = 0;
	fps = 0;
	fpsLast = SDL_GetTicks();

	return true;
}

// Creates a buffer texture for our drawing surface.
bool SDLMan::createBuffer() {
	// create the texture to use as our buffer
	mBuffer = std::make_unique<Texture>(SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mBufferW, mBufferH));
	if (mBuffer == nullptr) return false;

	// point the render target at the buffer so all drawers are drawingt to the buffer and not SDLs built in backbuffer
	if (TRIPLE_BUFFERING) SDL_SetRenderTarget(mRenderer, mBuffer->getTexture());

	return true;
}

// Called upon completion of a window resize event. Adjusts window to proper aspect ratio for the game.
void SDLMan::updateWindowSize() {
	// get new size
	SDL_GetWindowSize(mWindow, &mWindowW, &mWindowH);
	std::cout << "Original W/H: " << mWindowW << ", " << mWindowH << "\n";


	// Calculate new destination rectangle height our buffer will be drawn into. Width will be window width.
	double newAspect = static_cast<double>(mWindowW) / static_cast<double>(mWindowH);

	// Calculate window size to maintain aspect ratio
	if (newAspect > FuGlobals::ASPECT_RATIO) {
		mWindowH = static_cast<int>((1.f / FuGlobals::ASPECT_RATIO) * mWindowW);
	} else if (newAspect < FuGlobals::ASPECT_RATIO) {
		mWindowW = static_cast<int>((1.f / FuGlobals::ASPECT_RATIO) * mWindowH);
	}

	// Update window with new size and destination rectangle our buffer get's scaled too
	SDL_SetWindowSize(mWindow, mWindowW, mWindowH);
	dest = { 0, 0, mWindowW, mWindowH };

	//***DEBUG***
	std::cout << "New Wind W/H: " << mWindowW << ", " << mWindowH << "\n";
	std::cout << "Dest: " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
}

// Renders to the screen the contents of the buffer
void SDLMan::refresh() {
	// If triple buffering copy our own texture buffer to SDLs internal buffer
	if (TRIPLE_BUFFERING) {
		SDL_SetRenderTarget(mRenderer, NULL);
		SDL_RenderCopyEx(mRenderer,	mBuffer->getTexture(), NULL, &dest, 0, NULL, SDL_FLIP_NONE);
	}

	// Flip SDL's internal buffer to the window and clear buffer
	SDL_RenderPresent(mRenderer);
	SDL_RenderClear(mRenderer);

	// Repoint renderer back to our buffer and clear it if we are using triple buffering
	if (TRIPLE_BUFFERING) {
		SDL_SetRenderTarget(mRenderer, mBuffer->getTexture());
		SDL_RenderClear(mRenderer);
	}
}

// Show or hide the window. Returns false if SDL hasn't been initialized yet.
bool SDLMan::showWindow(bool win) {
	// first check if the window is nullptr
	if (mWindow == nullptr) {
		std::cerr << "Failed in SDLMan::showWindow: Attempted to show a null SDL_Window." << std::endl;
		return false;
	}

	// toggle the window visability and set the destination draw rectangle for our buffer to be drawn onto the window with a call to updateWindowSize()
	if (win) {
		SDL_ShowWindow(mWindow);
		updateWindowSize();
		SDL_RaiseWindow(mWindow);
		SDL_AddEventWatch(resizingEventWatcher, mWindow);
	} else {
		SDL_DelEventWatch(resizingEventWatcher, mWindow);
		SDL_HideWindow(mWindow);
	}	

	return true;
}

// Maintain aspect ratio upon window resize. Called from a SDL_AddEventWatch call injected in showWindow()
int SDLMan::resizingEventWatcher(void* data, SDL_Event* event) {
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
		SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
		if (win == (SDL_Window*)data) {
			
		}
	}

	return 0;
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

// Load in a Texture object using the passed in filename and SDL_Image functions. Returns a smart pointer to a Texture object.
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
	
	// Wrap the SDL_Texture in our Texture class and return as a smart pointer
	return std::make_unique<Texture>(newTexture);
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