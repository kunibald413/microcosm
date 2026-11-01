#include "view.h"
#include <math.h> // cos, sin

enum { 
    CIRCLE_SIDES = 16,
    INDICES_PER_TRIANGLE = 3
};

static const double MATH_PI = 3.14159265358979323846;
static const double MATH_TAU = 2.0 * MATH_PI;

static void draw_circle_fill(View *v, const Agent *a)
{
    SDL_Vertex verts[CIRCLE_SIDES + 1];
    int idx[CIRCLE_SIDES * INDICES_PER_TRIANGLE];

    verts[0].position.x = (float)a->X;
    verts[0].position.y = (float)a->Y;
    verts[0].color.r = (float)a->r;
    verts[0].color.g = (float)a->g;
    verts[0].color.b = (float)a->b;
    verts[0].color.a = 1.0f;
    verts[0].tex_coord.x = 0.0f;
    verts[0].tex_coord.y = 0.0f;

    for (int i = 0; i < CIRCLE_SIDES; i++) {
        const double ang = (double)i / (double)CIRCLE_SIDES * MATH_TAU;
        verts[i + 1].position.x = (float)(a->X + cos(ang) * a->Radius);
        verts[i + 1].position.y = (float)(a->Y + sin(ang) * a->Radius);
        verts[i + 1].color = verts[0].color;
        verts[i + 1].tex_coord.x = 0.0f;
        verts[i + 1].tex_coord.y = 0.0f;

        idx[i * INDICES_PER_TRIANGLE + 0] = 0;
        idx[i * INDICES_PER_TRIANGLE + 1] = i + 1;
        idx[i * INDICES_PER_TRIANGLE + 2] = ((i + 1) % CIRCLE_SIDES) + 1;
    }

    SDL_RenderGeometry(v->Renderer, NULL, verts, 
        CIRCLE_SIDES + 1, 
        idx, 
        CIRCLE_SIDES * INDICES_PER_TRIANGLE);
}

static void draw_circle_outline(View *v, const Agent *a)
{
    SDL_SetRenderDrawColor(v->Renderer, 0, 0, 0, 255);

    for (int i = 0; i < CIRCLE_SIDES; i++) {
        const double a0 = (double)i / (double)CIRCLE_SIDES * MATH_TAU;
        const double a1 =(double)(i + 1) / (double)CIRCLE_SIDES * MATH_TAU;
        const float x0 = (float)(a->X + cos(a0) * a->Radius);
        const float y0 = (float)(a->Y + sin(a0) * a->Radius);
        const float x1 = (float)(a->X + cos(a1) * a->Radius);
        const float y1 = (float)(a->Y + sin(a1) * a->Radius);

        SDL_RenderLine(v->Renderer, x0, y0, x1, y1);
    }
}

static const double SPIKE_REACH = 3.0;
static void draw_spike(View *v, const Agent *a)
{
    const double len = SPIKE_REACH * a->Radius * a->SpikeLength;
    const float sx = (float)(a->X + len * cos(a->Angle));
    const float sy = (float)(a->Y + len * sin(a->Angle));

    SDL_SetRenderDrawColor(v->Renderer, 128, 0, 0, 255);
    SDL_RenderLine(v->Renderer, (float)a->X, (float)a->Y, sx, sy);
}

bool View_Init(View *v, const char *title, int width, int height)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }

    v->Window = NULL;
    v->Renderer = NULL;

    //SDL_WindowFlags win_flags = SDL_WINDOW_RESIZABLE;
    SDL_WindowFlags win_flags = 0;
    if (!SDL_CreateWindowAndRenderer(title, width, height, win_flags, 
            &v->Window, &v->Renderer)) {
        SDL_Quit();
        return false;
    }

    return true;
}

void View_Destroy(View *v)
{
    SDL_DestroyRenderer(v->Renderer);
    v->Renderer = NULL;
    SDL_DestroyWindow(v->Window);
    v->Window = NULL;
    SDL_Quit();
}

void View_Clear(View *v)
{
    SDL_SetRenderDrawColor(v->Renderer, 0x1E, 0x1E, 0x28, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(v->Renderer);
}

void View_Present(View *v)
{
    SDL_RenderPresent(v->Renderer);
}

void View_Draw_Agent(View *v, const Agent *a)
{
    draw_circle_fill(v, a);
    draw_circle_outline(v, a);
    draw_spike(v, a);
}