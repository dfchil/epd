#ifndef SCENE_TRANSITION_H
#define SCENE_TRANSITION_H

#include <enDjinn/enj_enDjinn.h>


typedef struct change_scene_mode_s {
  void *prev_scene;
  void *next_scene;
} scene_transition_mode_t;

void scene_transition_updater(void* data);

enj_mode_t* scene_transition_get(void);

#endif // SCENE_TRANSITION_H