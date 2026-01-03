#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <enDjinn/enj_enDjinn.h>
#include <sh4zam/shz_sh4zam.h>
#include <stdint.h>

typedef struct game_player_s {
  shz_vec2_t position;
  uint16_t health;
  enj_color_t color;
  float shoot_angle;
  float shoot_power;
  int cooldown_timer;
  enj_abstract_ctrlr_t controller;
  void *scene;
} game_player_t;

int player_update(game_player_t* player);

#endif  // GAME_PLAYER_H
