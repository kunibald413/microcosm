#include <SDL3/SDL.h>      // SDL_Event, SDL_GetTicks, SDL_Delay
#include <SDL3/SDL_main.h> // SDL_main
#include <stdbool.h>
#include "agent.h"
#include "view.h"
#include "world.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const int screen_width = 640;
    const int screen_height = 480;

    View view;
    if (!View_Init(&view, "microcosm",
                   screen_width, screen_height)) {
        SDL_Log("View_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    World world;
    World_Init(&world, 3, 
        (double)screen_width,
        (double)screen_height);


    Uint64 last = SDL_GetTicks();
    bool quit = false;

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        const Uint64 now = SDL_GetTicks();
        const double dt = (double)(now - last) / 1000.0;
        last = now;

        World_Update(&world, dt);

        View_Clear(&view);
        View_Draw_World(&view, &world);
        View_Present(&view);

        SDL_Delay(16);
    }

    View_Destroy(&view);
    return 0;
}