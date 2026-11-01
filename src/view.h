#ifndef VIEW_H
#define VIEW_H

#include <stdbool.h>
#include <SDL3/SDL.h> // SDL_Window, SDL_Renderer
#include "agent.h"
#include "world.h"

typedef struct {
    SDL_Window *Window;
    SDL_Renderer *Renderer;
} View;

bool View_Init(View *v, const char *title, int width, int height);
void View_Destroy(View *v);
void View_Clear(View *v);
void View_Present(View *v);
void View_Draw_Agent(View *v, const Agent *a);
void View_Draw_World(View *v, const World *w);

#endif