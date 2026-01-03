#include <gorgol8/game/scene.h>
#include <gorgol8/modes/scene_transition.h>
#include <gorgol8/render/scene.h>

#define CS_PIXELS_PER_STEP 3

alignas(32) static scene_transition_mode_t s_change_scene_mode = {0};
void scene_transition_updater(void* data) {
  scene_transition_mode_t* cs_data = (scene_transition_mode_t*)data;
  scene_t* prev_scene = (scene_t*)(cs_data->prev_scene);
  scene_t* next_scene = (scene_t*)(cs_data->next_scene);
  prev_scene->offset_x -= CS_PIXELS_PER_STEP;
  next_scene->offset_x -= CS_PIXELS_PER_STEP;
  if (next_scene->offset_x <= 0) {
    enj_mode_pop();
    enj_mode_get()->data = next_scene;
    scene_demolish(prev_scene);
  } else {
    render_scene(prev_scene);
    render_scene(next_scene);
  }
}

alignas(32) static enj_mode_t change_scene_mode = {
    .name = "Change Scene Mode",
    .mode_updater = scene_transition_updater,
    .on_activation_fn = NULL,
    .data = &s_change_scene_mode,
};

enj_mode_t* scene_transition_get(void) {
  return &change_scene_mode;
}