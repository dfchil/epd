#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/game/package.h>
#include <mortarlity/game/terrain.h>
#include <mortarlity/game/confetti.h>

#include <mortarlity/modes/scene_transition.h>
#include <mortarlity/render/scene.h>
#include <mortarlity/render/terrain.h>

#include <kos/timer.h>
#include <malloc.h>
#include <stdlib.h>

void scene_demolish(scene_t *scene) {
  if (scene) {
    if (scene->terrain) {
      free(scene->terrain);
      scene->terrain = NULL;
    }
    free(scene);
  } else {
    ENJ_DEBUG_PRINT("scene_demolish: Attempted to demolish NULL scene!");
  }
}

scene_t *scene_construct(int num_players, scene_t *prev_scene) {
  float prev_last_y = -1.0f;
  if (prev_scene && prev_scene->terrain) {
    prev_last_y =
        prev_scene->terrain->verts[prev_scene->terrain->num_verts - 1].y;
  }

  scene_t *new_scene = memalign(32, sizeof(scene_t));
  if (!new_scene) {
    return NULL;
  }
  new_scene->num_players = num_players;
  new_scene->offset_x = 0;

  uint32_t secs, nsecs;
  timer_ns_gettime(&secs, &nsecs);
  srand(nsecs);
  new_scene->terrain = terrain_generate(
      num_players, 5.0f * ((rand() % 1000 / 1000.0f) + 0.3f), prev_last_y);

  enj_ctrlr_state_t **cstates = enj_ctrl_get_states();

  // randomize start positions
  for (int i = 0; i < num_players; i++) {
    int frompos = rand() % (num_players - i);
    shz_vec2_t pos = new_scene->terrain->player_positions[i];
    new_scene->terrain->player_positions[i] =
        new_scene->terrain->player_positions[frompos + i];
    new_scene->terrain->player_positions[frompos + i] = pos;
    player_initialize(i, new_scene);

    if (cstates[i] != NULL) {
      new_scene->players[i].controller.updatefun =
          enj_read_dreamcast_controller;
      new_scene->players[i].controller.state =
          &(new_scene->players[i].controller);
      new_scene->players[i].controller.port = i;
    }
  }
  return new_scene;
}

static inline void _next_scene(scene_t *current_scene) {
  enj_mode_t *cs_mode = scene_transition_get();
  scene_transition_mode_t *cs_mode_data =
      (scene_transition_mode_t *)cs_mode->data;
  cs_mode_data->prev_scene = current_scene;
  scene_t *next_scene =
      scene_construct(current_scene->num_players, current_scene);
  next_scene->offset_x = vid_mode->width;
  cs_mode_data->next_scene = next_scene;
  enj_mode_push(cs_mode);
  enj_mode_get()->mode_updater(enj_mode_get()->data);
}

void scene_updater(void *data) {
  scene_t *scene = (scene_t *)data;
  enj_ctrlr_state_t **ctrl_states = enj_ctrl_get_states();
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (ctrl_states[i] != NULL) {
      if (ctrl_states[i]->button.Y == ENJ_BUTTON_DOWN_THIS_FRAME) {
        _next_scene(scene);
        return;
      }
    }
  }
  int num_alive = 0;
  for (int i = 0; i < scene->num_players; i++) {
    num_alive += player_update(scene->players + i);
  }

  pkg_t *pkg = package_get_first();
  while (pkg != NULL) {
    pkg_t *next_pkg = pkg->next;
    if (!package_update(pkg, 8.0f / 60.0f)) {
      package_destroy(pkg);
    }
    pkg = next_pkg;
  }

  confetti_cluster_t *confetti = confetti_get_first();
  while (confetti != NULL) {
    confetti_cluster_t *next_confetti = confetti->next;
    if (!confetti_update(confetti)) {
      confetti_destroy(confetti);
    }
    confetti = next_confetti;
  }

  if (num_alive <= 1) {
    _next_scene(scene);
    return;
  }
  render_scene(scene);
}
