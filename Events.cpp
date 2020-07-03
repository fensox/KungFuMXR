#include "Events.h"
#include "SDL.h"

void Events::doEvents() {
    SDL_Event e;

    // Poll SDL events
    while (SDL_PollEvent(&e))
    {
        //User requests quit
        if (e.type == SDL_QUIT)
        {
            mQuit = true;
        }
        //User presses a key
        else if (e.type == SDL_KEYDOWN)
        {
            //Select surfaces based on key press
            switch (e.key.keysym.sym)
            {
            case SDLK_UP:

                break;

            case SDLK_DOWN:

                break;

            case SDLK_LEFT:

                break;

            case SDLK_RIGHT:

                break;

            default:

                break;
            }
        }
    }
}

bool Events::getQuit() {
    return mQuit;
}