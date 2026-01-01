#ifndef GAME_TERRAIN_H
#define GAME_TERRAIN_H

#include <sh4zam/shz_sh4zam.h>
#include <stddef.h>

typedef struct terrain_s {
  size_t num_verts;
  shz_vec2_t* verts;
  float min_y;
  float max_y;
  int seed;
  float roughness;
  shz_vec2_t player_positions[8];
} terrain_t;

terrain_t* terrain_generate(int num_players, int seed, float roughness);
#endif
