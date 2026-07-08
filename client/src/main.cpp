#include "camera.hpp"
#include "config.hpp"
#include "input.hpp"
#include "islewright/common/version.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>
#include <iostream>

using namespace islewright;

int main(int argc, char* argv[])
{
    // Print the project name and version
    std::cout << common::project_name() << " client " << common::project_version() << '\n';

    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create an application window with the following settings
    SDL_Window* window =
        SDL_CreateWindow(config::WINDOW_TITLE, config::WINDOW_WIDTH, config::WINDOW_HEIGHT,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // Check that the window was successfully created
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    input::InputState input;
    camera::Camera camera;

    // Initialize the camera with the current window size
    int windowWidth = 0, windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    camera.SetViewportSize(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    Uint64 previousTicks = SDL_GetTicks();

    // Main loop
    while (!input.QuitRequested()) {
        input.BeginFrame();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            input.HandleEvent(event);
        }

        // Calculate delta time
        const Uint64 currentTicks = SDL_GetTicks();
        const float dt = static_cast<float>(currentTicks - previousTicks) / 1000.0f;
        previousTicks = currentTicks;

        // Handle camera movement and zoom
        if (input.WindowResized()) {
            camera.SetViewportSize(static_cast<float>(input.WindowWidth()),
                                   static_cast<float>(input.WindowHeight()));
        }

        // Adjust pan speed based on delta time and camera zoom level
        const float panSpeed = 500.0f * dt / camera.Zoom();

        if (input.KeyDown(SDL_SCANCODE_A) || input.KeyDown(SDL_SCANCODE_LEFT)) {
            camera.MoveBy({.x = -panSpeed, .y = 0.0f});
        }

        if (input.KeyDown(SDL_SCANCODE_D) || input.KeyDown(SDL_SCANCODE_RIGHT)) {
            camera.MoveBy({.x = panSpeed, .y = 0.0f});
        }

        if (input.KeyDown(SDL_SCANCODE_W) || input.KeyDown(SDL_SCANCODE_UP)) {
            camera.MoveBy({.x = 0.0f, .y = -panSpeed});
        }

        if (input.KeyDown(SDL_SCANCODE_S) || input.KeyDown(SDL_SCANCODE_DOWN)) {
            camera.MoveBy({.x = 0.0f, .y = panSpeed});
        }

        // Handle mouse wheel zooming
        if (input.WheelY() != 0.0f) {
            camera.ZoomAt({.x = input.MouseX(), .y = input.MouseY()},
                          std::pow(1.1f, input.WheelY()));
        }
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    return EXIT_SUCCESS;
}
