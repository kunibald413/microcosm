#include "base.h"  // u32, u64
#include "view.h"
#include "math_const.h"
#include <math.h> // cos, sin

enum { 
    CIRCLE_SIDES = 16,
    INDICES_PER_TRIANGLE = 3
};

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

    if (!SDL_SetRenderVSync(v->Renderer, 1)) {
        SDL_Log("vsync not set: %s", SDL_GetError());
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

void View_LogInfo(const View *v)
{
    const int version = SDL_GetVersion();
    SDL_Log("SDL %d.%d.%d",
        SDL_VERSIONNUM_MAJOR(version),
        SDL_VERSIONNUM_MINOR(version),
        SDL_VERSIONNUM_MICRO(version));

    SDL_PropertiesID props =
        SDL_GetRendererProperties((SDL_Renderer *)v->Renderer);
    const char *name = SDL_GetStringProperty(
        props, SDL_PROP_RENDERER_NAME_STRING, "unknown");
    const bool is_software = (SDL_strcmp(name, "software") == 0);
    SDL_Log("Renderer: %s (%s)", name,
        is_software ? "CPU" : "GPU");
}

void View_LogWorldSize(const World *w)
{
    (void)w;   // reserved for future per-world fields

    const u64 bytes = (u64)sizeof(World);
    const u64 food = (u64)sizeof(((World *)0)->Food);

    const double bytes_per_mb = 1024.0 * 1024.0;
    SDL_Log("World: %llu bytes (%.2f MB)", bytes,
        (double)bytes / bytes_per_mb);
    SDL_Log("Food grid: %llu bytes (%.2f MB)", food,
        (double)food / bytes_per_mb);
}

void View_Draw_Agent(View *v, const Agent *a)
{
    draw_circle_fill(v, a);
    draw_circle_outline(v, a);
    draw_spike(v, a);
}

void View_Draw_Food(View *v, const World *w)
{
    SDL_SetRenderDrawColor(v->Renderer, 0x40, 0x80, 0x40, 255);

    for (int y = 0; y < w->FoodRows; y++) {
        for (int x = 0; x < w->FoodCols; x++) {
            if (w->Food[y][x] <= 0.0f) {
                continue;
            }

            SDL_FRect r = {
                (float)(x * FOOD_CELL_SIZE),
                (float)(y * FOOD_CELL_SIZE),
                (float)FOOD_CELL_SIZE,
                (float)FOOD_CELL_SIZE
            };
            SDL_RenderFillRect(v->Renderer, &r);
        }
    }
}

void View_Draw_World(View *v, const World *w)
{
    View_Draw_Food(v, w);
    for (int i = 0; i < MAX_AGENTS; i++) {
        if (!w->Active[i]) {
            continue;
        }
        View_Draw_Agent(v, &w->Agents[i]);
    }
}