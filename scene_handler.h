#ifndef SCENE_HANDLER_H
#define SCENE_HANDLER_H

#include "transition_handler.h"

#define NO_SCENE -1


int add_scene(const char *scene_name, bool (*init_method)(void), void (*render_method)(void), bool (*run_method)(void), void (*end_method)(void), TRANSITION_TYPE transition_type);

bool set_scene(int scene_pos);
bool first_scene(void);
bool next_scene(void);
bool run_scene(void);

int find_scene_pos(char *scene_name);

#endif
