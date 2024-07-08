#include "src/include/SDL2/SDL.h"
// #include "SDL.h"

class SDL_Handler
{
private:
    /* data */
public:
    ~SDL_Handler();

    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    SDL_Window *Window;

    SDL_Surface *screenSurface;

    SDL_Renderer *Renderer;

    SDL_Event *e;

    // initialise window, returns true if everything worked
    bool init();
    // clean up sdl
    void cleanUp();
    // render board
    void renderBackground();
    // Clear the screen
    void wipeScreen();
};