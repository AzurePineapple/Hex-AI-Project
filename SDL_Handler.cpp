#include "SDL_Handler.h"

SDL_Handler::~SDL_Handler()
{
}

void SDL_Handler::renderBackground()
{
    bool white = true;
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (white)
            {
                SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(Renderer, 155, 103, 60, 255);
            }
            white = !white;
            SDL_Rect rectangle = {i * SCREEN_WIDTH / 8, j * SCREEN_HEIGHT / 8, SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8};

            SDL_RenderDrawRect(Renderer, &rectangle);
        }
        white = !white;
    }
}

void SDL_Handler::wipeScreen()
{
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderClear(Renderer);
    SDL_RenderPresent(Renderer);
}

void SDL_Handler::showMainScreen()
{
    // load image as a surface
    SDL_Surface *loadedSurface = IMG_Load("images/Idle Screen 1.png");
    if (loadedSurface == NULL)
    {
        std::cout << "Unable to load image" << std::endl;
        return;
    }

    // Convert image to texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
    if (texture == NULL)
    {
        std::cout << "Could not create texture from surface" << std::endl;
        SDL_FreeSurface(loadedSurface);
        return;
    }

    // Free the surface
    SDL_FreeSurface(loadedSurface);

    // Clear the screen
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderClear(Renderer);

    // Render the texture to the screen
    SDL_RenderCopy(Renderer, texture, NULL, NULL);

    // Update the screen
    SDL_RenderPresent(Renderer);

    // Free the texture
    SDL_DestroyTexture(texture);
}

void SDL_Handler::displayLoading()
{
    SDL_Surface *loadedSurface = IMG_Load("images/LoadingIndicator.png");
    if (loadedSurface == NULL)
    {
        std::cout << "Unable to load image" << std::endl;
        return;
    }

    // Convert image to texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
    if (texture == NULL)
    {
        std::cout << "Could not create texture from surface" << std::endl;
        SDL_FreeSurface(loadedSurface);
        return;
    }

    // Get image dimensions
    int imgWidth = loadedSurface->w / 4;
    int imgHeight = loadedSurface->h / 4;

    // Free the surface
    SDL_FreeSurface(loadedSurface);

    SDL_Rect renderCorner;
    renderCorner = {SCREEN_WIDTH - imgWidth,
                    0,
                    imgWidth,
                    imgHeight};

    SDL_RenderCopy(Renderer, texture, NULL, &renderCorner);

    SDL_RenderPresent(Renderer);

    SDL_DestroyTexture(texture);
}

void SDL_Handler::cleanUp()
{
    SDL_FreeSurface(screenSurface);
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
}

bool SDL_Handler::init()
{
    Window = NULL;
    screenSurface = NULL;

    bool quit = false;

    // initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialise! SDL Error: " << SDL_GetError() << std::endl;
        cleanUp();
        return false;
    }
    else
    {
        // Set samples for multisampling
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

        // create window
        Window = SDL_CreateWindow("Hex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_WINDOW_ALLOW_HIGHDPI));
        if (Window == NULL)
        {
            std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
            cleanUp();
            return false;
        }
        else
        {
            Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
            // Dark mode
            SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
            // Light mode
            // SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

            SDL_RenderClear(Renderer);
            SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        }
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cout << "SDL_Image could not initialise! SDL_Image Error: %s\n"
                  << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}
