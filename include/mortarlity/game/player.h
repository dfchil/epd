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


typedef enum : uint8_t {
  NO_DRIFT = 0,
  DRIFTING_LEFT = 1,
  DRIFTING_RIGHT = 2
} drift_dir_e;

typedef struct game_player_s {
  void *scene;
  shz_vec2_t position;
  player_col_def_t color;
  float shoot_angle;
  float shoot_power;
  int cooldown_timer;
  enj_abstract_ctrlr_t controller;
  shz_vec2_t aiming_at;
  union {
    uint8_t raw;
    struct {
      uint8_t left : 1;
      uint8_t right : 1;
      uint8_t bottom : 1;
      uint8_t reserved : 5;
    }
  } aim_exit_screen;
  union {
    uint8_t raw;
    struct {
      drift_dir_e aim_drifting : 2;
      uint8_t terrain_hit : 1;
      uint8_t stuck_on_acent : 1;  // hit vertical obstacle going up
      uint8_t stuck_on_decent : 1; // means terrain is shadowed by an obstacle
      uint8_t reserved : 3;
    }
  } aim_flags;

} game_player_t;

void player_initialize(int player_index, void *scene);

int player_update(game_player_t *player);

player_col_def_t player_color_get(int player_index);

void player_setup_colors();

#endif // GAME_PLAYER_H
