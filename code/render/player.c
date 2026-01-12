#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/collision.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/arrow.h>
#include <mortarlity/render/player.h>
#include <mortarlity/render/primitives.h>
#include <sh4zam/shz_sh4zam.h>

#define num_trajectory_points 1000
static inline void _render_trajectory(game_player_t* player) {
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);
  float power_scale = player->shoot_power * 10.0f;

  shz_vec2_t points[num_trajectory_points];
  float time_step = 0.5f;

  shz_vec2_t fire_start = {
      .x = player->position.x + (barrel.cos * BARREL_OFFSET),
      .y = player->position.y + (barrel.sin * BARREL_OFFSET),
  };

  for (int i = 0; i < num_trajectory_points; i++) {
    float t = time_step * (float)i;
    points[i] = (shz_vec2_t){
        .x = fire_start.x + (barrel.cos * power_scale * t),
        .y = fire_start.y + (barrel.sin * power_scale * t) -
             (0.5f * 9.81f * t * t),
    };
    if (points[i].y < 0.0f) {
      return render_strip_line(
          points, i,
          &(shz_vec3_t){.x = (float)((scene_t*)player->scene)->offset_x - 0.5f,
                        .y = 0.0f,
                        .z = 1.0f},
          1.0f,
          (enj_color_t){.r = player->color.primary.r,
                        .g = player->color.primary.g,
                        .b = player->color.primary.b,
                        .a = 0x50},
          PVR_LIST_TR_POLY, NULL);
    }
  }
  render_strip_line(
      points, num_trajectory_points,
      &(shz_vec3_t){.x = (float)((scene_t*)player->scene)->offset_x - 0.5f,
                    .y = 0.0f,
                    .z = 1.0f},
      1.0f, (enj_color_t){.raw = 0x80FFFF00}, PVR_LIST_TR_POLY, NULL);
}

static void _render_player_TR(void* data) {
  game_player_t* player = (game_player_t*)data;
  render_arrow(player);
  // _render_trajectory(player);
}

static void _render_player_OP(void* data) {
  game_player_t* player = (game_player_t*)data;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(state);
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = player->color.primary.raw;
  pvr_dr_commit(hdr);

  /**
   *   0 ______ 1
   *     \    /
   *    3 ---- 2
   */
  const float offset_x = (float)((scene_t*)player->scene)->offset_x;

  const float pmodel[4][3] = {
      {(player->position.x + offset_x - 7.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y -1.0f, 10.0f},  // 0
      {(player->position.x + offset_x + 6.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y -1.0f, 10.0f},  // 1
      {(player->position.x + offset_x + 3.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y + 5.0f, 10.0f},  // 2
      {(player->position.x + offset_x - 4.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y + 5.0f, 10.0f}};

  enj_draw_sprite(pmodel, &state, NULL, NULL);
}

void render_player(game_player_t* player) {
  render_arrow(player);
  // calculate arrow vertices
  enj_render_list_add(PVR_LIST_OP_POLY, _render_player_OP, player);
  enj_render_list_add(PVR_LIST_TR_POLY, _render_player_TR, player);
}
