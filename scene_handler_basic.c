#include <string.h>
#include <stdbool.h>

#include "scene_handler_basic.h"

#define RETURN_IF_FALSE(function)  if (function() == false) { return false; }

#define MAX_SCENE_NAME_LEN 50
#define MAX_SCENE_ENTRIES 20

static int num_scenes = 0;
static int current_scene_pos = NO_SCENE;

typedef struct
{
    char name[MAX_SCENE_NAME_LEN];
    bool (*init_method)(void);
    bool (*run_method)(void);
    void (*end_method)(void);
} SCENE_ENTRY;

static SCENE_ENTRY scene_entries[MAX_SCENE_ENTRIES];


static bool init_scene(void);
static void end_scene(void);


int add_scene(const char *scene_name, bool (*init_method)(void), bool (*run_method)(void), void (*end_method)(void))
{
    int scene_pos = NO_SCENE;

    if (num_scenes < MAX_SCENE_ENTRIES)
    {
        SCENE_ENTRY scene_entry;

        strncpy(scene_entry.name, scene_name, MAX_SCENE_NAME_LEN);
        scene_entry.init_method = init_method;
        scene_entry.run_method = run_method;
        scene_entry.end_method = end_method;

        scene_entries[num_scenes] = scene_entry;
        scene_pos = num_scenes++;
    }

    return scene_pos;
}

bool set_scene(int scene_pos)
{
    if (num_scenes == 0)
    {
        return false;
    }

    current_scene_pos = scene_pos;    

    return init_scene();
}

bool first_scene()
{
    if (num_scenes == 0)
    {
        return false;
    }

    current_scene_pos = 0;

    return init_scene();
}

//  This can be called instead of first_scene(), which is just for syntactical nicety
bool next_scene()
{
    if (num_scenes == 0)
    {
        return false;
    }

    if (current_scene_pos != NO_SCENE)
    {
        end_scene();
    }

    if (++current_scene_pos > num_scenes)
    {
        current_scene_pos = 0;
    }

    return init_scene();
}

bool run_scene(void)
{
    return scene_entries[current_scene_pos].run_method();
}

int find_scene_pos(char *scene_name)
{
    int scene_pos = NO_SCENE;

    for(int pos = 0; pos < MAX_SCENE_ENTRIES; pos++)
    {
        if (strncmp(scene_name, scene_entries[pos].name, MAX_SCENE_NAME_LEN) == 0)
        {
            scene_pos = pos;
            break;
        }
    }

    return scene_pos;
}

static bool init_scene(void)
{
    if (scene_entries[current_scene_pos].init_method == NULL)
    {
        //  Is ok not to have an initialisation function
        return true;
    }

    return scene_entries[current_scene_pos].init_method();
}

static void end_scene(void)
{
    if (scene_entries[current_scene_pos].end_method != NULL)
    {
        //  Is ok not to have an cleanup function
        scene_entries[current_scene_pos].end_method();
    }
}
