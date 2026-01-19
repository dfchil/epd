#include <mortarlity/game/collision.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/primitives.h>
#include <mortarlity/render/trajectory.h>
#include <sh4zam/shz_sh4zam.h>

#define num_trajectory_points 127
#define TRAJECTORY_WIDTH 0.75f
void render_trajectory_TR(game_player_t *player) {
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);
  const terrain_t *terrain = ((scene_t *)player->scene)->terrain;

  shz_vec2_t points[num_trajectory_points];
  const float time_step = 0.5f;

  int i = 0;
  points[i] = (shz_vec2_t){
      .x = player->position.x + (barrel.cos * BARREL_OFFSET),
      .y = player->position.y + (barrel.sin * BARREL_OFFSET),
  };
  float collision_delta = -1.0f;
  int terrain_index_0, terrain_index_1;

  for (i = 1; i < num_trajectory_points; i++) {
    if (collision_delta > 0.0f) {
      break;
    }
    const float t = time_step * (float)i;
    points[i] = (shz_vec2_t){
        .x = points[0].x + (barrel.cos * player->shoot_power * t),
        .y = points[0].y + (barrel.sin * player->shoot_power * t) -
             (time_step * 9.81f * t * t),
    };

    if (points[i].x < 0.0f || points[i].x > vid_mode->width) {
      terrain_index_0 = -1;
      terrain_index_1 = -1;
      i++;
      break;
    }

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
        terrain_index_0 = tidx;
        terrain_index_1 = tidx + 1;
        points[i + 1] = points[i];
        points[i] =
            shz_vec2_add(points[i - 1], shz_vec2_scale(dv0, collision_delta));
        break;
      }
    }
  }
  render_strip_line(
      points, i,
      &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x - 0.5f,
                    .y = 0.0f,
                    .z = 33.0f},
      TRAJECTORY_WIDTH,
      (enj_color_t){.r = player->color.primary.r,
                    .g = player->color.primary.g,
                    .b = player->color.primary.b,
                    .a = 64},
      PVR_LIST_TR_POLY, NULL);

  //   if (player->controller.port == ENJ_PORT_A && terrain_index_0 >= 0 &&
  //       terrain_index_1 <= terrain->num_verts) {
  //     render_strip_line(
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