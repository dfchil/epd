#ifndef GAME_SHELL_H
#define GAME_SHELL_H

#include <mortarlity/game/player.h>

#define SHELL_SIMULATION_STEPS 10
#define SHELL_MOTION_BLUR_STEPS 96

typedef struct shell_s {
  shz_vec2_t position;
  shz_vec2_t velocity;
  shz_vec2_t trail[SHELL_MOTION_BLUR_STEPS];

  struct shell_s *prev;
  struct shell_s *next;

  int32_t frame;
  uint16_t used_trail_steps;
  uint16_t trail_fade; 
  struct {
    uint16_t moving: 1;
    uint16_t reserved : 15;
  };
  game_player_t *origin;

  uint32_t shadow_min_x;
  uint32_t shadow_max_x;
} shell_t;

int shell_modulo(int x, int N);

shell_t* shell_get_first();

shell_t *shell_create(float pos_x, float pos_y, float vel_x, float vel_y,
                      game_player_t *origin);
int shell_update(shell_t *shell, float delta_time);

void shell_destroy(shell_t *shell);

#endif // GAME_SHELL_H