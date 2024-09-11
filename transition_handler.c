#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

#include "transition_handler.h"

//  Constants from OpenGL
#define GL_SRC_ALPHA 0x0302
#define GL_MIN 0x8007

typedef struct
{
    float duration;
    Texture2D start_texture;
    Texture2D end_texture;
    RenderTexture2D transition_texture;
    float start_value;
    float end_value;
} TRANSITION_DATA;

static bool transition_active = false;
static TRANSITION_DATA data;
static float transition_duration = DEFAULT_TRANSITION_DURATION;

static Image start_screen;
static Texture2D end_screen;
static RenderTexture2D screen_texture;

static struct timeval transition_start_time;

static void (*current_transition)(void); 
static void init_fade(void);
static void run_fade(void);
static void draw_fade(Color start_tint, Color end_tint);
static void init_slide_left_overlap(void);
static void run_slide_left_overlap(void);
static void draw_slide_left_overlap(float end_x);
static void init_slide_right_overlap(void);
static void run_slide_right_overlap(void);
static void draw_slide_right_overlap(float end_x);
static void init_slide_left(void);
static void run_slide_left(void);
static void draw_slide_left(float start_x, float end_x);
static void init_slide_right(void);
static void run_slide_right(void);
static void draw_slide_right(float start_x, float end_x);
static void init_circle_expand(void);
static void run_circle_expand(void);
static void draw_circle_expand(float radius);
static void init_circle_contract(void);
static void run_circle_contract(void);
static void draw_circle_contract(float radius);
static void end_transition(void);

static void set_transition_start_time(void);
static double get_transition_time_delta(void);


void set_transition_duration(float duration)
{
    transition_duration = duration;
}

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
    screen_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    
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

        case TRANSITION_CIRCLE_EXPAND:
            transition_active = true;
            init_circle_expand();
            break;

        case TRANSITION_CIRCLE_CONTRACT:
            transition_active = true;
            init_circle_contract();
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

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = (RenderTexture2D){ 0 };
    data.start_value = 255;
    data.end_value = 0;

    set_transition_start_time();
}

static void run_fade(void)
{
    Color start_tint = WHITE;
    Color end_tint = WHITE;
    float time_delta = (float)get_transition_time_delta();
    float variance = (255 / (data.duration / time_delta));
    float start_variance = data.start_value - variance;
    float end_variance = data.end_value + variance;

    if (start_variance < 0.0f || end_variance > 255.0f)
    {
        start_tint.a = (unsigned char)data.end_value;
        end_tint.a = (unsigned char)data.start_value;

        draw_fade(start_tint, end_tint);
        end_transition();
    }
    else
    {
        start_tint.a = (unsigned char)start_variance;
        end_tint.a = (unsigned char)end_variance;

        draw_fade(start_tint, end_tint);
    }
}

static void draw_fade(Color start_tint, Color end_tint)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, start_tint);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ 0, 0 }, 0.0f, end_tint);
    EndDrawing();
}

static void init_slide_left_overlap(void)
{
    current_transition = &run_slide_left_overlap;

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = (RenderTexture2D){ 0 };
    data.start_value = (float)GetScreenWidth() * -1.0f;
    data.end_value = 0.0f;

    set_transition_start_time();
}

static void run_slide_left_overlap(void)
{
    float time_delta = (float)get_transition_time_delta();
    float variance = (GetScreenWidth() / (data.duration / time_delta));
    float start_variance = data.start_value + variance;

    if (start_variance >= data.end_value)
    {
        draw_slide_left_overlap(data.end_value);
        end_transition();
    }
    else
    {
        draw_slide_left_overlap(start_variance);
    }
}

static void draw_slide_left_overlap(float end_x)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ end_x, 0 }, 0.0f, WHITE);
    EndDrawing();
}

static void init_slide_right_overlap(void)
{
    current_transition = &run_slide_right_overlap;

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = (RenderTexture2D){ 0 };
    data.start_value = (float)GetScreenWidth();
    data.end_value = 0.0f;

    set_transition_start_time();
}

static void run_slide_right_overlap(void)
{
    float time_delta = (float)get_transition_time_delta();
    float variance = (data.start_value / (data.duration / time_delta));
    float start_variance = data.start_value - variance;

    if (start_variance <= data.end_value)
    {
        draw_slide_right_overlap(data.end_value);
        end_transition();
    }
    else
    {
        draw_slide_right_overlap(start_variance);
    }
}

static void draw_slide_right_overlap(float end_x)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ end_x, 0 }, 0.0f, WHITE);
    EndDrawing();
}

static void init_slide_left(void)
{
    current_transition = &run_slide_left;

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = (RenderTexture2D){ 0 };
    data.start_value = (float)GetScreenWidth() * -1.0f;
    data.end_value = 0.0f;

    set_transition_start_time();
}

static void run_slide_left(void)
{
    float time_delta = (float)get_transition_time_delta();
    float variance = ((float)GetScreenWidth() / (data.duration / time_delta));
    float start_variance = data.start_value + variance;

    if (start_variance >= data.end_value)
    {
        draw_slide_left(data.start_value, data.end_value);
        end_transition();
    }
    else
    {
        float start_x = data.start_value - start_variance;
        float end_x = start_variance;

        draw_slide_left(start_x, end_x);
    }
}

static void draw_slide_left(float start_x, float end_x)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, start_x, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ end_x, 0 }, 0.0f, WHITE);
    EndDrawing();
}

static void init_slide_right(void)
{
    current_transition = &run_slide_right;

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = (RenderTexture2D){ 0 };
    data.start_value = (float)GetScreenWidth();
    data.end_value = 0.0f;

    set_transition_start_time();
}

static void run_slide_right(void)
{
    float time_delta = (float)get_transition_time_delta();
    float variance = (data.start_value / (data.duration / time_delta));
    float start_variance = data.start_value - variance;

    if (start_variance <= data.end_value)
    {
        draw_slide_right(data.start_value, data.end_value);
        end_transition();
    }
    else
    {
        float start_x = data.start_value - start_variance;
        float end_x = start_variance;

        draw_slide_right(start_x, end_x);
    }
}

static void draw_slide_right(float start_x, float end_x)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, start_x, 0, WHITE);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ end_x, 0 }, 0.0f, WHITE);
    EndDrawing();
}

static void init_circle_expand(void)
{
    float x = (float)GetScreenWidth();
    float y = (float)GetScreenHeight();
    float radius = 0.5f * sqrtf((x * x) + (y * y));

    current_transition = &run_circle_expand;

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = LoadRenderTexture(x, y);
    data.start_value = 0.0f;
    data.end_value = radius;

    set_transition_start_time();
}

static void run_circle_expand(void)
{
    float time_delta = (float)get_transition_time_delta();
    float variance = ((float)data.end_value / (data.duration / time_delta));
    float current_radius = data.start_value + variance;

    if (current_radius >= data.end_value)
    {
        draw_circle_expand(data.end_value);
        end_transition();
    }
    else
    {
        draw_circle_expand(current_radius);
    }
}

static void draw_circle_expand(float radius)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginTextureMode(data.transition_texture);
        ClearBackground(BLACK);
        DrawTexture(data.start_texture, 0, 0, WHITE);

        // Force the blend mode to only set the alpha of the destination
        rlSetBlendFactors(GL_SRC_ALPHA, GL_SRC_ALPHA, GL_MIN);
        rlSetBlendMode(BLEND_CUSTOM);

        // Draw a blank 'hole' in our texture
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, radius, BLANK);

        // Go back to normal
        rlSetBlendMode(BLEND_ALPHA);
    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawTexturePro(data.transition_texture.texture, rect_end_source, rect_end_dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

static void init_circle_contract(void)
{
    float x = (float)GetScreenWidth();
    float y = (float)GetScreenHeight();
    float radius = 0.5f * sqrtf((x * x) + (y * y));

    current_transition = &run_circle_contract;

    data.duration = transition_duration;
    data.start_texture = LoadTextureFromImage(start_screen);
    data.end_texture = end_screen;
    data.transition_texture = LoadRenderTexture(x, y);
    data.start_value = radius;
    data.end_value = 0.0f;

    set_transition_start_time();
}

static void run_circle_contract(void)
{
    float time_delta = (float)get_transition_time_delta();
    float variance = ((float)data.start_value / (data.duration / time_delta));
    float current_radius = data.start_value - variance;

    if (current_radius <= data.end_value)
    {
        draw_circle_contract(data.end_value);
        end_transition();
    }
    else
    {
        draw_circle_contract(current_radius);
    }
}

static void draw_circle_contract(float radius)
{
    // RenderTextures have an opposite Y axis
    Rectangle rect_end_source = (Rectangle){ 0, 0, (float)data.end_texture.width, -(float)data.end_texture.height };
    Rectangle rect_end_dest = (Rectangle){ 0, 0, (float)data.end_texture.width, (float)data.end_texture.height };

    BeginTextureMode(data.transition_texture);
        ClearBackground(BLACK);
        DrawTexturePro(data.end_texture, rect_end_source, rect_end_dest, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Force the blend mode to only set the alpha of the destination
        rlSetBlendFactors(GL_SRC_ALPHA, GL_SRC_ALPHA, GL_MIN);
        rlSetBlendMode(BLEND_CUSTOM);

        // Draw a blank 'hole' in our texture
        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, radius, BLANK);

        // Go back to normal
        rlSetBlendMode(BLEND_ALPHA);
    EndTextureMode();

    BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(data.start_texture, 0, 0, WHITE);
        DrawTexturePro(data.transition_texture.texture, rect_end_source, rect_end_dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

static void end_transition(void)
{
    UnloadImage(start_screen);
    UnloadTexture(end_screen);

    UnloadTexture(data.start_texture);
    UnloadTexture(data.end_texture);  // Extraneous as end_screen is assigned to data.end_texture

    UnloadRenderTexture(data.transition_texture);
    UnloadRenderTexture(screen_texture);

    transition_active = false;
}

static void set_transition_start_time(void)
{
    gettimeofday(&transition_start_time, NULL);
}

static double get_transition_time_delta(void)
{
    struct timeval transition_end_time;
    double delta_usecs;
    double delta;

    gettimeofday(&transition_end_time, NULL);

    delta_usecs = (double)(transition_end_time.tv_usec - transition_start_time.tv_usec) / 1000000.0;
    delta = (double)(transition_end_time.tv_sec - transition_start_time.tv_sec) + delta_usecs;

    return delta;
}
