#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/scene.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/render/terrain.h>
#include <gorgol8/render/scene.h>

#include <kos/timer.h>
#include <stdlib.h>
#include <malloc.h>

void scene_demolish(scene_t* scene) {
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

scene_t* scene_construct(int num_players) {
  scene_t* new_scene = memalign(32, sizeof(scene_t));
  if (!new_scene) {
    return NULL;
  }
  new_scene->num_players = num_players;
  new_scene->offset_x = 0;

  uint32_t secs, nsecs;
  timer_ns_gettime(&secs, &nsecs);
  srand(nsecs);
  new_scene->terrain = terrain_generate(num_players, 5.0f * ((rand() % 1000 / 1000.0f) + 0.5f), 0.0f);

  // randomize start positions
  for (int i = 0; i < num_players; i++) {
    int frompos = rand() % (num_players - i);
    shz_vec2_t pos = new_scene->terrain->player_positions[i];
    new_scene->terrain->player_positions[i] =
        new_scene->terrain->player_positions[frompos + i];
    new_scene->terrain->player_positions[frompos + i] = pos;

    new_scene->players[i].position = new_scene->terrain->player_positions[i];
    new_scene->players[i].health = 100;
    new_scene->players[i].color = _player_colors[i % NUM_PLAYER_COLORS];
    new_scene->players[i].barrel_angle = 0.0f;
    new_scene->players[i].barrel_length = 20.0f;
    new_scene->players[i].cooldown_timer = 0;
    new_scene->players[i].controller.port = i;
    new_scene->players[i].controller.type = 0;
  }
  return new_scene;
}

void scene_updater(void* data) {
  scene_t* scene = (scene_t*)data;
  enj_ctrlr_state_t ** ctrl_states = enj_ctrl_get_states();
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (ctrl_states[i] != NULL) {
      if (ctrl_states[i]->button.A == ENJ_BUTTON_DOWN_THIS_FRAME) {
        scene_demolish(scene);
        scene = scene_construct(8);
        enj_mode_get()->data = scene;
      }
    }
  }
  for (int i = 0; i < scene->num_players; i++) {
    player_update(scene->players + i);
  }
  render_scene(scene);
}
