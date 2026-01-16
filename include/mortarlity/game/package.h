#ifndef GAME_PACKAGE_H
#define GAME_PACKAGE_H

#include <mortarlity/game/player.h>

#define PACKAGE_SIMULATION_STEPS 10
#define PACKAGE_TRAIL_STEPS 96

typedef struct pkg_s {
  shz_vec2_t position;
  shz_vec2_t velocity;
  shz_vec2_t trail[PACKAGE_TRAIL_STEPS];

  struct pkg_s *prev;
  struct pkg_s *next;

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
} pkg_t;

int package_modulo(int x, int N);

pkg_t* package_get_first();

pkg_t *package_create(float pos_x, float pos_y, float vel_x, float vel_y,
                      game_player_t *origin);
int package_update(pkg_t *pkg, float delta_time);

void package_destroy(pkg_t *pkg);

#endif // GAME_PACKAGE_H