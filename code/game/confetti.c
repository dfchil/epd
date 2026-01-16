#include <malloc.h>
#include <mortarlity/game/confetti.h>
#include <mortarlity/render/confetti.h>
#include <stdlib.h>

static confetti_cluster_t *first_confetti = NULL;
static confetti_cluster_t *last_confetti = NULL;

confetti_cluster_t *confetti_get_first() { return first_confetti; }

confetti_cluster_t *confetti_create(shz_vec2_t position, enj_color_t color_src,
                                    enj_color_t color_dst, int num_particles,
                                    float force, int lifetime) {

  confetti_cluster_t *new_confetti =
      memalign(32, sizeof(confetti_cluster_t) +
                       num_particles * sizeof(confetti_particle_t));
  if (!new_confetti) {
    return NULL;
  }
  new_confetti->particles = (confetti_particle_t *)(new_confetti + 1);
  new_confetti->position = position;
  for (int i = 0; i < num_particles; i++) {
    float angle = (float)(40 + rand() % 100) * (3.14159f / 180.0f);
    shz_sincos_t angle_sincos = shz_sincosf(angle);
    float speed = (0.5f + ((rand() % 1000) / 1000.0f)) * force;
    new_confetti->particles[i].velocity = (shz_vec2_t){
        .x = angle_sincos.cos * speed, .y = angle_sincos.sin * speed};
    new_confetti->particles[i].position =
        shz_vec2_add(position, new_confetti->particles[i].velocity);

    new_confetti->particles[i].rotation = 0.0f;
    new_confetti->particles[i].rotation_speed =
        ((rand() % 2000) / 1000.0f - 1.0f) * 5.0f; // -5 to +5
    new_confetti->particles[i].size = 1.0f;
  }
  new_confetti->color_src = color_src;
  new_confetti->color_dst = color_dst;
  new_confetti->lifetime = lifetime;
  new_confetti->age = 0;
  new_confetti->num_particles = num_particles;

  if (first_confetti == NULL) {
    first_confetti = new_confetti;
    last_confetti = new_confetti;
    new_confetti->prev = NULL;
    new_confetti->next = NULL;
  } else {
    last_confetti->next = new_confetti;
    new_confetti->prev = last_confetti;
    new_confetti->next = NULL;
    last_confetti = new_confetti;
  }
  return new_confetti;
}

void confetti_destroy(confetti_cluster_t *confetti) {
  if (confetti) {
    if (confetti->prev) {
      confetti->prev->next = confetti->next;
    } else {
      first_confetti = confetti->next;
    }
    if (confetti->next) {
      confetti->next->prev = confetti->prev;
    } else {
      last_confetti = confetti->prev;
    }
    free(confetti);
  }
}

int confetti_update(confetti_cluster_t *confetti) {
  confetti->age++;
  if (confetti->age >= confetti->lifetime) {
    return 0; // Indicate that the confetti should be destroyed
  }
  for (int i = 0; i < confetti->num_particles; i++) {
    confetti->particles[i].position = shz_vec2_add(
        confetti->particles[i].position, confetti->particles[i].velocity);
    confetti->particles[i].rotation += confetti->particles[i].rotation_speed;
    // Apply simple gravity
    confetti->particles[i].velocity.y -= 0.05f;
  }
  render_confetti(confetti);
  return 1; // Confetti is still active
}
