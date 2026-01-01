#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <enDjinn/enj_enDjinn.h>
#include <sh4zam/shz_sh4zam.h>
#include <stdint.h>

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

static const char* _color_names[NUM_PLAYER_COLORS] = {
    "BLUE", "GOLD", "RED ", "GREEN", "PURPLE", "SILVER", "PINK", "TEAL"};

typedef struct game_player_s {
  shz_vec2_t position;
  uint16_t health;
  enj_color_t color;
  float barrel_angle;
  float barrel_length;
  int cooldown_timer;
  enj_abstract_ctrlr_t controller;
} game_player_t;

void player_update(game_player_t* player);

#endif  // GAME_PLAYER_H
