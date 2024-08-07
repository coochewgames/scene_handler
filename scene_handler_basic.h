#ifndef SCENE_HANDLER_H
#define SCENE_HANDLER_H

#define NO_SCENE -1

int add_scene(const char *scene_name, bool (*init_method)(void), bool (*run_method)(void), void (*end_method)(void));

bool set_scene(int scene_pos);
bool first_scene();
bool next_scene();
bool run_scene();

int find_scene_pos(char *scene_name);

#endif
