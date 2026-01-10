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

float collision_shell_terrain(shell_t *shell, terrain_t *terrain,
                              float delta_time) {
  //   shz_vec2_t start_pos = shell->position;
  //   shz_vec2_t end_pos = shz_vec2_add(
  //       shell->position, shz_vec2_scale(shell->velocity, delta_time));

  //   shz_vec2_t movement_vec = shz_vec2_sub(end_pos, start_pos);

  //   for (int i = 0; i < terrain->num_segments; i++) {
  //     terrain_segment_t *segment = &terrain->segments[i];
  //     float collision_t = collision_line_line(
  //         start_pos, movement_vec,
  //         segment->start_pos,
  //         shz_vec2_sub(segment->end_pos, segment->start_pos));
  //     if (collision_t >= 0.0f) {
  //       return collision_t;
  //     }
  //   }

  return -1.0f;
}