#ifndef TRANSITION_HANDLER_H
#define TRANSITION_HANDLER_H

#include <raylib.h>

typedef enum
{
    TRANSITION_NONE = 0,
    TRANSITION_FADE,
    TRANSITION_SLIDE_LEFT_OVERLAP,
    TRANSITION_SLIDE_RIGHT_OVERLAP,
    TRANSITION_SLIDE_LEFT,
    TRANSITION_SLIDE_RIGHT,
    TRANSITION_ALL
} TRANSITION_TYPE;

bool is_transition_active(void);

void set_transition_start_screen(void);
void set_transition_end_screen(void (*render_method)(void));
void start_transition(TRANSITION_TYPE type);
void run_transition(void);
TRANSITION_TYPE get_random_transition(void);

#endif
