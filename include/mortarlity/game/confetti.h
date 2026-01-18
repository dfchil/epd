#ifndef GAME_CONFETTI_H
#define GAME_CONFETTI_H

#include <enDjinn/enj_enDjinn.h>
#include <sh4zam/shz_sh4zam.h>

typedef struct confetti_particle_s {
  shz_vec2_t position;
  shz_vec2_t velocity;
  shz_vec3_t rotation;
  float rotation_speed;
  float start_angle;
} confetti_particle_t;

typedef struct confetti_cluster_s {
  shz_vec2_t position;
  shz_vec2_t velocity;
  enj_color_t color_src;
  enj_color_t color_dst;
  float rotation;
  float rotation_speed;
  float size;
  int lifetime;
  int age;
  int num_particles;
  struct confetti_cluster_s *prev;
  struct confetti_cluster_s *next;
  confetti_particle_t *particles;
} confetti_cluster_t;

confetti_cluster_t *confetti_get_first();

confetti_cluster_t *confetti_create(shz_vec2_t position, enj_color_t color_src,
                                    enj_color_t color_dst, int num_particles,
                                    float force, int lifetime);

void confetti_destroy(confetti_cluster_t *confetti);

int confetti_update(confetti_cluster_t *confetti);

#endif // GAME_CONFETTI_H