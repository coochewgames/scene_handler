#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "transition_handler.h"

typedef struct
{
    float duration;
    Texture2D start_texture;
    Texture2D end_texture;
    float start_value;
    float end_value;
} TRANSITION_DATA;


static bool transition_active = false;
static TRANSITION_DATA data;

static Image start_screen;
static Texture2D end_screen;

static void (*current_transition)(void); 
static void init_fade(void);
static void run_fade(void);
static void init_slide_left_overlap(void);
static void run_slide_left_overlap(void);
static void init_slide_right_overlap(void);
static void run_slide_right_overlap(void);
static void init_slide_left(void);
static void run_slide_left(void);
static void init_slide_right(void);
static void run_slide_right(void);


bool is_transition_active(void)
{
    return transition_active;
}

void set_transition_start_screen(void)
{
    start_screen = LoadImageFromScreen();
}

void set_transition_end_screen(void (*render_method)(void))
{
    RenderTexture2D screen_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    
    BeginTextureMode(screen_texture);
        render_method();
    EndTextureMode();

    end_screen = screen_texture.texture;
}

void start_transition(TRANSITION_TYPE type)
{
    switch(type)
    {
        case TRANSITION_FADE:
            transition_active = true;
            init_fade();
            break;

        case TRANSITION_SLIDE_LEFT_OVERLAP:
            transition_active = true;
            init_slide_left_overlap();
            break;

        case TRANSITION_SLIDE_RIGHT_OVERLAP:
            transition_active = true;
            init_slide_right_overlap();
            break;

        case TRANSITION_SLIDE_LEFT:
            transition_active = true;
            init_slide_left();
            break;

        case TRANSITION_SLIDE_RIGHT:
            transition_active = true;
            init_slide_right();
            break;

        default:
            transition_active = false;
    }
}

void run_transition(void)
{
    current_transition();
}

TRANSITION_TYPE get_random_transition(void)
{
    static int has_run = 0;
    int max = TRANSITION_ALL;

    if (has_run == 0)
    {
        srand((unsigned int)time(0));
        has_run = 1;
    }

     return (TRANSITION_TYPE)(rand() % (int)TRANSITION_ALL);
}

static void init_fade(void)
{
    current_transition = &run_fade;

    data.duration = 2.0f;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.start_value = 255;
    data.end_value = 0;
}

static void run_fade(void)
{
    Color start_tint = WHITE;
    Color end_tint = WHITE;
    float time_delta = GetFrameTime();
    float variance = (255 / (data.duration / time_delta));

    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    start_tint.a = (unsigned char)data.start_value;
    end_tint.a = (unsigned char)data.end_value;

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, start_tint);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ 0, 0 }, 0.0f, end_tint);
    EndDrawing();

    data.start_value -= variance;
    data.end_value += variance;

    if (data.start_value < 0.0f || data.end_value > 255.0f)
    {
        transition_active = false;
    }
}

static void init_slide_left_overlap(void)
{
    current_transition = &run_slide_left_overlap;

    data.duration = 2.0f;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.start_value = (float)GetScreenWidth() * -1.0f;
    data.end_value = 0.0f;
}

static void run_slide_left_overlap(void)
{
    float time_delta = GetFrameTime();
    float variance = (GetScreenWidth() / (data.duration / time_delta));
    float x = data.start_value;

    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ x, 0 }, 0.0f, WHITE);
    EndDrawing();

    data.start_value += variance;

    if (data.start_value >= data.end_value)
    {
        transition_active = false;
    }
}

static void init_slide_right_overlap(void)
{
    current_transition = &run_slide_right_overlap;

    data.duration = 2.0f;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.start_value = (float)GetScreenWidth();
    data.end_value = 0.0f;
}

static void run_slide_right_overlap(void)
{
    float time_delta = GetFrameTime();
    float variance = (GetScreenWidth() / (data.duration / time_delta));
    float x = data.start_value;

    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ x, 0 }, 0.0f, WHITE);
    EndDrawing();

    data.start_value -= variance;

    if (data.start_value <= data.end_value)
    {
        transition_active = false;
    }
}

static void init_slide_left(void)
{
    current_transition = &run_slide_left;

    data.duration = 2.0f;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.start_value = (float)GetScreenWidth() * -1.0f;
    data.end_value = 0.0f;
}

static void run_slide_left(void)
{
    int width = GetScreenWidth();
    float time_delta = GetFrameTime();
    float variance = ((float)width / (data.duration / time_delta));
    float start_x = ((float)width * -1.0f) - data.start_value;
    float end_x = data.start_value;

    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, start_x, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ end_x, 0 }, 0.0f, WHITE);
    EndDrawing();

    data.start_value += variance;

    if (data.start_value >= data.end_value)
    {
        transition_active = false;
    }
}

static void init_slide_right(void)
{
    current_transition = &run_slide_right;

    data.duration = 2.0f;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.start_value = (float)GetScreenWidth();
    data.end_value = 0.0f;
}

static void run_slide_right(void)
{
    int width = GetScreenWidth();
    float time_delta = GetFrameTime();
    float variance = ((float)width / (data.duration / time_delta));
    float start_x = (float)width - data.start_value;
    float end_x = data.start_value;

    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, start_x, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ end_x, 0 }, 0.0f, WHITE);
    EndDrawing();

    data.start_value -= variance;

    if (data.start_value <= data.end_value)
    {
        transition_active = false;
    }
}
