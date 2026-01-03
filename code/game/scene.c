#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/scene.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/render/terrain.h>
#include <gorgol8/render/scene.h>
#include <gorgol8/modes/scene_transition.h>

#include <kos/timer.h>
#include <stdlib.h>
#include <malloc.h>


static enj_color_t _player_colors[] = {
    {.raw = 0xff14a5ff},  // blue
    {.raw = 0xffffc010},  // light orange/gold
    {.raw = 0xffff450a},  // red
    {.raw = 0xff0fff50},  // neon green
    {.raw = 0xff9c27ff},  // Purple
    {.raw = 0xffc9c0bb},  // silver
    {.raw = 0xffff6ec7},  // neon pink
    {.raw = 0xff009b7d},  // teal
};
#define NUM_PLAYER_COLORS (sizeof(_player_colors) / sizeof(enj_color_t))

// static const char* _color_names[NUM_PLAYER_COLORS] = {
//     "BLUE", "GOLD", "RED ", "GREEN", "PURPLE", "SILVER", "PINK", "TEAL"};


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

scene_t* scene_construct(int num_players, scene_t* prev_scene) {
  float prev_last_y = 0.0f;
  if (prev_scene && prev_scene->terrain) {
    prev_last_y = prev_scene->terrain->verts[prev_scene->terrain->num_verts - 1].y;
  }

  scene_t* new_scene = memalign(32, sizeof(scene_t));
  if (!new_scene) {
    return NULL;
  }
  new_scene->num_players = num_players;
  new_scene->offset_x = 0;

  uint32_t secs, nsecs;
  timer_ns_gettime(&secs, &nsecs);
  srand(nsecs);
  new_scene->terrain = terrain_generate(num_players, 5.0f * ((rand() % 1000 / 1000.0f) + 0.5f), prev_last_y);

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
    new_scene->players[i].scene = new_scene;
  }
  return new_scene;
}

void scene_updater(void* data) {
  scene_t* scene = (scene_t*)data;
  enj_ctrlr_state_t ** ctrl_states = enj_ctrl_get_states();
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (ctrl_states[i] != NULL) {
      if (ctrl_states[i]->button.A == ENJ_BUTTON_DOWN_THIS_FRAME) {
        enj_mode_t* cs_mode = scene_transition_get();
        scene_transition_mode_t* cs_mode_data = (scene_transition_mode_t*)cs_mode->data;
        cs_mode_data->prev_scene = scene;
        cs_mode_data->next_scene = scene_construct(scene->num_players, scene);
        ((scene_t*)cs_mode_data->next_scene)->offset_x = vid_mode->width;
        enj_mode_push(cs_mode);
        enj_mode_get()->mode_updater(enj_mode_get()->data);
        return;
      }
    }
  }
  for (int i = 0; i < scene->num_players; i++) {
    player_update(scene->players + i);
  }
  render_scene(scene);
}

