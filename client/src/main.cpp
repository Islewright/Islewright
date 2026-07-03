#include "config.hpp"
#include "islewright/common/version.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

using namespace islewright::config;

int main(int argc, char* argv[])
{
    // Initialize SDL3
    SDL_Init(SDL_INIT_VIDEO);

    // Create an application window with the following settings
    SDL_Window* window =
        SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    // Check that the window was successfully created
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    bool done = false;

    // Main loop
    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        // TODO: Do game logic, present a frame, etc.
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    return EXIT_SUCCESS;
}
