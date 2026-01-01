#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/modes/scene.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/render/terrain.h>

#include <kos/timer.h>
#include <stdlib.h>

scene_t scene_data = {
    .terrain = NULL,
};

static enj_mode_t scene = {
    .name = "scene mode",
    .name = "The main mode",
    .on_activation_fn = NULL,
    .data = &scene_data,
};
scene_t* scene_construct(int num_players) {
  if (scene_data.terrain) {
    free(scene_data.terrain);
  }
  uint32_t secs, nsecs;
  timer_ns_gettime(&secs, &nsecs);
  scene_data.terrain = terrain_generate(8, nsecs, 5.0f * ((rand() % 1000 / 1000.0f) + 0.5f));
  return &scene_data;
}

static void mode_single_updater(void* data) {
  scene_t* mode_data = (scene_t*)data;

  enj_ctrlr_state_t ** ctrl_states = enj_ctrl_get_states();
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (ctrl_states[i] != NULL) {
      if (ctrl_states[i]->button.A == ENJ_BUTTON_DOWN_THIS_FRAME) {
        scene_construct(8);
      }
    }
  }

  if (mode_data->terrain) {
    enj_render_list_add(PVR_LIST_OP_POLY, render_terrain, mode_data->terrain);
    enj_render_list_add(PVR_LIST_PT_POLY, render_terrain_stats, mode_data->terrain);
  }
}

enj_mode_t* scene_mode_get(void) {
  scene.mode_updater = mode_single_updater;
  return &scene;
}