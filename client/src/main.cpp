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
    SDL_Window* window = SDL_CreateWindow(config::WINDOW_TITLE, config::WINDOW_WIDTH,
                                          config::WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

    // Check that the window was successfully created
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Create a renderer for the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // Check that the renderer was successfully created
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    input::InputState input;
    camera::Camera camera;

    // Initialize the camera with the current window size
    int windowWidth = 0;
    int windowHeight = 0;
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
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            camera.SetViewportSize(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
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
            camera.ZoomAt({.x = input.MouseX(), .y = input.MouseY()}, input.WheelY() * 0.1f);
        }

        // Render the scene
        SDL_SetRenderDrawColor(renderer, 18, 18, 22, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);

        constexpr int halfGrid = 50;

        // Draw grid lines
        for (int i = -halfGrid; i <= halfGrid; ++i) {
            constexpr float tile = 32.0f;
            const float pos = static_cast<float>(i) * tile;

            // Draw vertical lines
            auto a = camera.WorldToScreen({.x = pos, .y = -halfGrid * tile});
            auto b = camera.WorldToScreen({.x = pos, .y = halfGrid * tile});
            SDL_RenderLine(renderer, a.x, a.y, b.x, b.y);

            // Draw horizontal lines
            a = camera.WorldToScreen({.x = -halfGrid * tile, .y = pos});
            b = camera.WorldToScreen({.x = halfGrid * tile, .y = pos});
            SDL_RenderLine(renderer, a.x, a.y, b.x, b.y);
        }

        // Draw the origin point
        SDL_SetRenderDrawColor(renderer, 220, 80, 80, 255);
        auto origin = camera.WorldToScreen({.x = 0.0f, .y = 0.0f});
        SDL_FRect originRect{.x = origin.x - 4.0f, .y = origin.y - 4.0f, .w = 8.0f, .h = 8.0f};
        SDL_RenderFillRect(renderer, &originRect);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    return EXIT_SUCCESS;
}
