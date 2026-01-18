#include <dc/video.h>
#include <malloc.h>
#include <mortarlity/game/collision.h>
#include <mortarlity/game/confetti.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/game/package.h>
#include <mortarlity/game/terrain.h>
#include <mortarlity/render/package.h>
#include <mortarlity/sfx/sounds.h>
#include <sh4zam/shz_sh4zam.h>

static pkg_t *first_pkg = NULL;
static pkg_t *last_pkg = NULL;

inline int package_modulo(int x, int N) { return (x % N + N) % N; }

pkg_t *package_get_first() { return first_pkg; }

pkg_t *package_create(float pos_x, float pos_y, float vel_x, float vel_y,
                      game_player_t *origin) {
  pkg_t *new_pkg = memalign(32, sizeof(pkg_t));
  if (!new_pkg) {
    return NULL;
  }
  new_pkg->position = (shz_vec2_t){.x = pos_x, .y = pos_y};
  new_pkg->velocity = (shz_vec2_t){.x = vel_x, .y = vel_y};
  for (int i = 0; i < PACKAGE_TRAIL_STEPS; i++) {
    new_pkg->trail[i].x = 0.0f;
    new_pkg->trail[i].y = 0.0f;
  }

  new_pkg->origin = origin;
  new_pkg->trail[PACKAGE_TRAIL_STEPS - 1] = new_pkg->position;
  new_pkg->frame = 1;
  new_pkg->used_trail_steps = 1;
  new_pkg->trail_fade = 0;
  new_pkg->moving = 1;

  new_pkg->shadow_min_x = UINT32_MAX;
  new_pkg->shadow_max_x = 0;

  if (first_pkg == NULL) {
    first_pkg = new_pkg;
    last_pkg = new_pkg;
    new_pkg->prev = NULL;
    new_pkg->next = NULL;
  } else {
    last_pkg->next = new_pkg;
    new_pkg->prev = last_pkg;
    new_pkg->next = NULL;
    last_pkg = new_pkg;
  }

  return new_pkg;
}

int package_update(pkg_t *pkg, float delta_time) {
  if (!pkg->moving) {
    pkg->trail_fade--;
    if (pkg->trail_fade == 0) {
      return 0; // Indicate that the pkg should be destroyed
    }
    render_pkg(pkg);
    return 1; // Shell is inactive but not destroyed
  }
  if (pkg->used_trail_steps < PACKAGE_TRAIL_STEPS) {
    pkg->used_trail_steps++;
  }
  pkg->frame++;
  // move backwards in the trail buffer
  int32_t current_frame = package_modulo(-pkg->frame, PACKAGE_TRAIL_STEPS);

  const float time_step = delta_time / (float)PACKAGE_SIMULATION_STEPS;
  const scene_t *scene = (scene_t *)pkg->origin->scene;
  const terrain_t *terrain = scene->terrain;

  pkg->shadow_min_x = UINT32_MAX;
  pkg->shadow_max_x = 0;

  for (int i = 0; i < PACKAGE_SIMULATION_STEPS; i++) {
    shz_vec2_t delta = shz_vec2_scale(pkg->velocity, time_step);

    for (int p = 0; p < scene->num_players; p++) {
      game_player_t *player = &scene->players[p];

      float collision_t =
          collision_player_line(player, &pkg->position, &delta);

      if (collision_t >= 0.0f) {
        // Collision detected
        pkg->position.x += delta.x * collision_t;
        pkg->position.y += delta.y * collision_t;
        pkg->trail[current_frame] = pkg->position;
        pkg->trail_fade = PACKAGE_TRAIL_STEPS;
        pkg->moving = 0;
        render_pkg(pkg);
        // Apply damage to the player
        float force = shz_vec2_magnitude(delta);
        printf("delta magnitude: %f\n", force);
        confetti_create(pkg->position, player->color.primary,
                        pkg->origin->color.primary, 50 + (int)(force * 20), 1.0f + force*0.5f, 90 + (int)(force * 30));

        if (player == pkg->origin) {
          // Self-hit, sarcastic sound
          sfx_play(SFX_SARCASTIC, 192, sfx_pos2pan(pkg->position.x));
        } else {
          // Hit another player, enthusiastic sound
          sfx_play(SFX_YAY_ENTHUSIASTIC, 32, sfx_pos2pan(pkg->position.x));
        }
        pkg->velocity = (shz_vec2_t){.x = 0.0f, .y = 0.0f};
        return 1; // Shell is still active
      }
    }

    // test for collision with terrain here
    int terrain_index_0 = -1;
    int terrain_index_1 = -1;

    terrain_index_0 =
        (int)(SHZ_MIN(pkg->position.x, pkg->position.x + delta.x) /
              (vid_mode->width / terrain->num_verts));
    terrain_index_1 =
        (int)(SHZ_MAX(pkg->position.x, pkg->position.x + delta.x) /
              (vid_mode->width / terrain->num_verts));
    terrain_index_0 -= 1;
    if (terrain_index_0 < 0) {
      terrain_index_0 = 0;
    }
    if (terrain_index_1 > terrain->num_verts - 1) {
      terrain_index_1 = terrain->num_verts - 1;
    }
    pkg->shadow_max_x = terrain_index_1 > pkg->shadow_max_x
                              ? terrain_index_1
                              : pkg->shadow_max_x;
    pkg->shadow_min_x = terrain_index_0 < pkg->shadow_min_x
                              ? terrain_index_0
                              : pkg->shadow_min_x;

    for (int t = terrain_index_0; t <= terrain_index_1; t++) {
      float collision_t = collision_line_line(
          &pkg->position, &delta, terrain->verts + t,
          &((shz_vec2_t){.x = terrain->verts[t + 1].x - terrain->verts[t].x,
                         .y = terrain->verts[t + 1].y - terrain->verts[t].y}));

      if (collision_t >= 0.0f) {
        // Collision detected
        pkg->position.x += delta.x * collision_t;
        pkg->position.y += delta.y * collision_t;
        pkg->trail[current_frame] = pkg->position;
        pkg->trail_fade = PACKAGE_TRAIL_STEPS;
        sfx_play(SFX_THUD, 64 + (int)(shz_vec2_magnitude(delta) * 64), sfx_pos2pan(pkg->position.x));
        pkg->moving = 0;
        render_pkg(pkg);
        pkg->velocity = (shz_vec2_t){.x = 0.0f, .y = 0.0f};
        return 1; // Shell is still active
      }
    }

    pkg->position.x += delta.x;
    pkg->position.y += delta.y;
    pkg->velocity.y -= 9.81f * time_step; // gravity
  }
  pkg->trail[current_frame] = pkg->position;

  // Check if the pkg has fallen below y = 0 (ground level)
  if (pkg->position.y < 0.0f) {
    pkg->trail_fade = PACKAGE_TRAIL_STEPS;
    pkg->moving = 0;
  }
  if (pkg->position.x < 0 || pkg->position.x > vid_mode->width) {
    pkg->trail_fade = PACKAGE_TRAIL_STEPS;
    pkg->moving = 0;
  }
  render_pkg(pkg);
  return 1; // Shell is still active
}

void package_destroy(pkg_t *pkg) {
  if (pkg->prev) {
    pkg->prev->next = pkg->next;
  } else {
    first_pkg = pkg->next;
  }
  if (pkg->next) {
    pkg->next->prev = pkg->prev;
  } else {
    last_pkg = pkg->prev;
  }
  free(pkg);
}
