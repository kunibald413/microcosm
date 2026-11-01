#include "base.h"          // u32, u64, MiB
#include <SDL3/SDL.h>      // SDL_Event, SDL_GetTicks, SDL_Delay
#include <SDL3/SDL_main.h> // SDL_main
#include <stdbool.h>
#include "agent.h"
#include "arena.h"
#include "view.h"
#include "world.h"

static const u64 FRAME_NS = 1000000000ULL / 60ULL;

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

    Arena *main_arena = Arena_Create(MiB(2));
    Arena_Log(main_arena);

    World *world = Arena_Push(main_arena, (u64)sizeof(World));
    World_Init(world, 4,
        (double)screen_width,
        (double)screen_height);

    View_LogInfo(&view);
    View_LogWorldSize(world);
    Arena_Log(main_arena);


    u64 last_ns = SDL_GetTicksNS();
    bool quit = false;

    while (!quit) {
        const u64 frame_start_ns = SDL_GetTicksNS();
        const double dt =
            (double)(frame_start_ns - last_ns) / 1000000000.0;
        last_ns = frame_start_ns;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        World_Update(world, dt);

        View_Clear(&view);
        View_Draw_World(&view, world);
        View_Present(&view);

        // frame cap, even if we have vsync enabled, just keep this
        const u64 elapsed_ns =
            SDL_GetTicksNS() - frame_start_ns;
        if (FRAME_NS > elapsed_ns) {
            SDL_DelayNS(FRAME_NS - elapsed_ns);
        }
    }

    View_Destroy(&view);
    Arena_Destroy(main_arena);
    return 0;
}