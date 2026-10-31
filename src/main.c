#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>

static const int kScreenWidth = 640;
static const int kScreenHeight = 480;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    /* Initialize SDL video */
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return 1;
    }

    /* Window + renderer in one call (0 == no extra window flags) */
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!SDL_CreateWindowAndRenderer("microcosm", kScreenWidth, kScreenHeight,
                                     0, &window, &renderer)) {
        SDL_Log("Window/renderer could not be created! SDL error: %s\n",
                SDL_GetError());
        SDL_Quit();
        return 1;
    }

    /* Main loop */
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        /* Process events */
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        /* Clear the screen */
        SDL_SetRenderDrawColor(renderer, 0x1E, 0x1E, 0x28, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        /* Draw something so we can tell the loop is really running */
        SDL_FRect rect = { 240.0f, 200.0f, 160.0f, 120.0f };
        SDL_SetRenderDrawColor(renderer, 0xE0, 0x50, 0x3C, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);

        /* Present the frame */
        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    /* Clean up */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
