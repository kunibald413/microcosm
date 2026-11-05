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
    verts[0].color.r = (float)a->Red;
    verts[0].color.g = (float)a->Green;
    verts[0].color.b = (float)a->Blue;
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

    v->LastFpsTime = SDL_GetTicks();
    v->FrameCount  = 0;
    v->Fps         = 0;

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

void View_UpdateFPS(View *v)
{
    v->FrameCount++;

    const u64 now = SDL_GetTicks();
    if (now - v->LastFpsTime >= 1000) {
        v->Fps         = v->FrameCount;
        v->FrameCount  = 0;
        v->LastFpsTime = now;
    }
}

void View_DrawStats(View *v, const World *w)
{
    const int alive = w->Count;
    const int dead  = w->DeathCount;

    char buf[128];
    snprintf(buf, sizeof(buf),
             "FPS %d  alive %d  dead %d",
             v->Fps, alive, dead);

    SDL_SetRenderScale(v->Renderer, 2.0f, 2.0f);
    SDL_SetRenderDrawColor(v->Renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(v->Renderer, 5.0f, 5.0f, buf);
    SDL_SetRenderScale(v->Renderer, 1.0f, 1.0f);
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

void View_Draw_Agent_Eyes(View *v, const Agent *a) {
    const double ang_l = a->Angle - EYE_OFFSET;
    const double ang_r = a->Angle + EYE_OFFSET;
    const double lx = a->X + cos(ang_l) * EYE_REACH;
    const double ly = a->Y + sin(ang_l) * EYE_REACH;
    const double rx = a->X + cos(ang_r) * EYE_REACH;
    const double ry = a->Y + sin(ang_r) * EYE_REACH;

    SDL_SetRenderDrawColor(v->Renderer, 128, 128, 128, 255);
    SDL_RenderLine(v->Renderer,
                   (float)a->X, (float)a->Y,
                   (float)lx,   (float)ly);
    SDL_RenderLine(v->Renderer,
                   (float)a->X, (float)a->Y,
                   (float)rx,   (float)ry);

    double tl = a->Sensors[SENSOR_LEFT];
    double tr = a->Sensors[SENSOR_RIGHT];
    if (tl > 1.0) tl = 1.0;
    if (tr > 1.0) tr = 1.0;

    SDL_SetRenderDrawColor(v->Renderer,
        (Uint8)(255.0 * tl), 0, 0, 255);
    SDL_FRect el = {
        (float)lx - 3.0f, (float)ly - 3.0f, 6.0f, 6.0f
    };
    SDL_RenderFillRect(v->Renderer, &el);

    SDL_SetRenderDrawColor(v->Renderer,
        (Uint8)(255.0 * tr), 0, 0, 255);
    SDL_FRect er = {
        (float)rx - 3.0f, (float)ry - 3.0f, 6.0f, 6.0f
    };
    SDL_RenderFillRect(v->Renderer, &er);
}

void View_Draw_Agent(View *v, const Agent *a)
{
    draw_circle_fill(v, a);
    draw_circle_outline(v, a);

    const double hp = a->Health / HEALTH_MAX;
    SDL_FRect bar = {
        (float)(a->X - a->Radius),
        (float)(a->Y + a->Radius + 4.0),
        (float)(a->Radius * 2.0 * hp),
        3.0f
    };
    SDL_SetRenderDrawColor(v->Renderer, 0, 220, 0, 255);
    SDL_RenderFillRect(v->Renderer, &bar);

    View_Draw_Agent_Eyes(v, a);

    draw_spike(v, a);
}

void View_Draw_Food(View *v, const World *w)
{
    for (int y = 0; y < w->FoodRows; y++) {
        for (int x = 0; x < w->FoodCols; x++) {
            if (w->Food[y][x] <= 0.0f) {
                continue;
            }

            const double t = (double)w->Food[y][x] / FOOD_MAX;
            const float cell = (float)FOOD_CELL_SIZE;
            const float side = cell * (float)t;
            const float pad = (cell - side) * 0.5f;

            SDL_SetRenderDrawColor(v->Renderer,
                (Uint8)(0x40 * t),
                (Uint8)(0x80 * t),
                (Uint8)(0x40 * t),
                255);

            SDL_FRect r = {
                (float)x * cell + pad,
                (float)y * cell + pad,
                side,
                side
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