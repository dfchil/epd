#include <mortarlity/game/collision.h>

#ifndef SHZ_ABS
#define SHZ_ABS(x) ((x) < 0 ? -(x) : (x))
#endif 

float collision_line_line(shz_vec2_t* l1_start, shz_vec2_t* l1_vec,
                          shz_vec2_t* l2_start, shz_vec2_t* l2_vec) {
  float det = -l2_vec->x * l1_vec->y + l1_vec->x * l2_vec->y;
  if (SHZ_ABS(det) < 0.00001f) {
    return -1.0f; // Lines are parallel
  }
  shz_vec2_t vec_diff = shz_vec2_sub(*l1_start, *l2_start);
  float s = (-l1_vec->y * vec_diff.x + l1_vec->x * vec_diff.y) / det;
  float t = (l2_vec->x * vec_diff.y - l2_vec->y * vec_diff.x) / det;

  if (s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f) {
    return t;
  }
  return -1.0f;
}

float collision_player_line(game_player_t* player, shz_vec2_t* line_start,
                            shz_vec2_t* line_vec) {

  const shz_vec2_t player_roof_start = {
      .x = player->position.x - 7.0f,
      .y = player->position.y,
  };
  const shz_vec2_t player_roof_vec = {
      .x = 13.0f,
      .y = 0.0f,
  };
  return collision_line_line(&player_roof_start, &player_roof_vec,
                               line_start, line_vec);
}