#include <mortarlity/game/collision.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/primitives.h>
#include <mortarlity/render/trajectory.h>
#include <sh4zam/shz_sh4zam.h>

#define MAX_TRAJECTORY_POINTS 192
#define TRAJECTORY_WIDTH 2.75f
#define GRAVITY_ACCELERATION 9.81f // pixels per second squared

static void _calc_trajectory_points(shz_vec2_t *points, game_player_t *player,
                                    int *first_terrain_index, int *exit_index) {
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);
  const terrain_t *terrain = ((scene_t *)player->scene)->terrain;
  const float time_step = 1.0f / 3.0f; // 30 FPS

  *first_terrain_index = -1;
  *exit_index = -1;
  
  points[0] = (shz_vec2_t){
      .x = player->position.x + (barrel.cos * BARREL_OFFSET),
      .y = player->position.y + (barrel.sin * BARREL_OFFSET),
  };
  float collision_delta = -1.0f;
  int terrain_index_0, terrain_index_1;
  player->aim_exit_screen.raw = 0;

  for (int i = 0; i < MAX_TRAJECTORY_POINTS; i++) {
    const float time = time_step * (float)i;
    points[i] = (shz_vec2_t){
        .x = points[0].x + barrel.cos * player->shoot_power * time,
        .y = points[0].y + barrel.sin * player->shoot_power * time -
             0.5f * GRAVITY_ACCELERATION * time * time,
    };
    if (points[i].x <= 1.0f &&
        player->aim_flags.aim_drifting == DRIFTING_LEFT) {
      player->aim_exit_screen.left = 1;
    }
    if (points[i].x >= vid_mode->width - 1.0f &&
        player->aim_flags.aim_drifting == DRIFTING_RIGHT) {
      player->aim_exit_screen.right = 1;
    }

    if (player->aim_exit_screen.left || player->aim_exit_screen.right) {
      if (!player->aim_flags.terrain_hit) {
        player->aiming_at = points[i];
      };
      *exit_index = i;
      break;
    }

    if (*first_terrain_index == -1) {
      terrain_index_0 = (int)(SHZ_MIN(points[i - 1].x, points[i].x) /
                              (vid_mode->width / terrain->num_verts)) -
                        1;
      terrain_index_1 = (int)(SHZ_MAX(points[i - 1].x, points[i].x) /
                              (vid_mode->width / terrain->num_verts)) +
                        1;

      if (terrain_index_0 >= terrain->num_verts) {
        terrain_index_0 = terrain->num_verts - 1;
      }
      if (terrain_index_1 >= terrain->num_verts) {
        terrain_index_1 = terrain->num_verts - 1;
      }
      if (terrain_index_0 < 0) {
        terrain_index_0 = 0;
      }
      if (terrain_index_0 >= terrain_index_1) {
        terrain_index_1 = terrain_index_0 + 1;
      }
      shz_vec2_t dv0 = shz_vec2_sub(points[i], points[i - 1]);
      for (int tidx = terrain_index_0; tidx < terrain_index_1; tidx++) {
        shz_vec2_t dv1 =
            shz_vec2_sub(terrain->verts[tidx + 1], terrain->verts[tidx]);
        collision_delta = collision_line_line(points + i - 1, &dv0,
                                              terrain->verts + tidx, &dv1);
        if (collision_delta > 0.0f) {
          player->aim_flags.terrain_hit = 1;
          points[i + 1] = points[i];
          points[i] =
              shz_vec2_add(points[i - 1], shz_vec2_scale(dv0, collision_delta));
          player->aiming_at = points[i];
          *first_terrain_index = i + 1;
        }
      }
    }
    if (points[i].y <= 0.0f) {
      player->aim_exit_screen.bottom = 1;
      player->aiming_at = points[i];
      *exit_index = i;
      break;
    }
  }
}

void render_trajectory_TR(game_player_t *player) {
  shz_vec2_t points[MAX_TRAJECTORY_POINTS];

  int first_terrain_index, exit_screen_index;

  _calc_trajectory_points(points, player, &first_terrain_index,
                          &exit_screen_index);

  render_strip_line_col(
      points, first_terrain_index,
      &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x - 0.5f,
                    .y = 0.0f,
                    .z = 33.0f + 0.001f * (float)player->controller.port},
      TRAJECTORY_WIDTH,
      (enj_color_t){.r = player->color.primary.r,
                    .g = player->color.primary.g,
                    .b = player->color.primary.b,
                    .a = 64},
      PVR_LIST_TR_POLY, NULL);

  if (exit_screen_index >= 0 && exit_screen_index > first_terrain_index) {
    render_strip_line_col(
        points + first_terrain_index - 1,
        exit_screen_index - first_terrain_index,
        &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x - 0.5f,
                      .y = 0.0f,
                      .z = 33.0f + 0.001f * (float)player->controller.port},
        TRAJECTORY_WIDTH,
        (enj_color_t){.r = player->color.contrast.r,
                      .g = player->color.contrast.g,
                      .b = player->color.contrast.b,
                      .a = 64},
        PVR_LIST_TR_POLY, NULL);
  }

  //   if (player->controller.port == ENJ_PORT_A && terrain_index_0 >= 0 &&
  //       terrain_index_1 <= terrain->num_verts) {
  //     render_strip_line_col(
  //         terrain->verts + terrain_index_0, terrain_index_1 - terrain_index_0
  //         + 1,
  //         &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x -
  //         0.5f,
  //                       .y = 0.0f,
  //                       .z = 33.0f},
  //         1.f,
  //         (enj_color_t){.r = player->color.primary.r,
  //                       .g = player->color.primary.g,
  //                       .b = player->color.primary.b,
  //                       .a = 0xff},
  //         PVR_LIST_TR_POLY, NULL);
  //   }
}