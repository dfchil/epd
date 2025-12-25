#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/modes/single.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/render/terrain.h>

#include <kos/timer.h>
#include <stdlib.h>

single_mode_t single_mode_data = {
    .terrain = NULL,
};

static enj_mode_t single = {
    .name = "single",
    .name = "The main mode",
    .pop_fun = NULL,
    .data = &single_mode_data,
};
void mode_single_regen(void) {
  if (single_mode_data.terrain) {
    free(single_mode_data.terrain);
  }
  uint32_t secs, nsecs;
  timer_ns_gettime(&secs, &nsecs);
  single_mode_data.terrain = terrain_generate(8, nsecs, 5.0f * ((rand() % 1000 / 1000.0f) + 0.5f));
}

static void mode_single_updater(void* data) {
  single_mode_t* mode_data = (single_mode_t*)data;

  enj_ctrlr_state_t ** ctrl_states = enj_ctrl_get_states();
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (ctrl_states[i] != NULL) {
      if (ctrl_states[i]->button.A == ENJ_BUTTON_DOWN_THIS_FRAME) {
        mode_single_regen();
      }
    }
  }

  if (mode_data->terrain) {
    enj_render_list_add(PVR_LIST_OP_POLY, render_terrain, mode_data->terrain);
    enj_render_list_add(PVR_LIST_PT_POLY, render_terrain_stats, mode_data->terrain);
  }
}


enj_mode_t* mode_single_get(void) {
  single.mode_updater = mode_single_updater;
  return &single;
}