#ifndef VIEW_H
#define VIEW_H

#include "base.h"  // u32, u64
#include <stdbool.h>
#include <SDL3/SDL.h> // SDL_Window, SDL_Renderer
#include "agent.h"
#include "world.h"

typedef struct {
    SDL_Window   *Window;
    SDL_Renderer *Renderer;
    u64           LastFpsTime;   // ms timestamp of last FPS sample
    int           FrameCount;    // frames since last sample
    int           Fps;           // last measured FPS
} View;

bool View_Init(View *v, const char *title, int width, int height);
void View_Destroy(View *v);
void View_Clear(View *v);
void View_Present(View *v);
void View_Draw_Agent(View *v, const Agent *a);
void View_Draw_Food(View *v, const World *w);
void View_Draw_World(View *v, const World *w);
void View_LogInfo(const View *v);
void View_LogWorldSize(const World *w);
void View_UpdateFPS(View *v);
void View_DrawStats(View *v, const World *w);

#endif