#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <enDjinn/enj_enDjinn.h>
#include <sh4zam/shz_sh4zam.h>
#include <stdint.h>

#define MAX_SHOOT_POWER 150.0f
#define MIN_SHOOT_POWER 30.0f
#define SHOT_COOLDOWN_FRAMES 150

#define BARREL_OFFSET 6.0f // offset from player position to barrel start


typedef struct {
  enj_color_t primary;
  enj_color_t contrast;
} player_col_def_t;


typedef struct game_player_s {
  void *scene;
  shz_vec2_t position;
  player_col_def_t color;
  float shoot_angle;
  float shoot_power;
  int cooldown_timer;
  enj_abstract_ctrlr_t controller;
  shz_vec2_t arrow_vertices[9];
  float x_drift;
  uint8_t aiming_at;
} game_player_t;

void player_initialize(int player_index, void *scene);

int player_update(game_player_t *player);

player_col_def_t player_color_get(int player_index);

void player_setup_colors();

#endif  // GAME_PLAYER_H
