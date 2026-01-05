#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/player.h>
#include <mortarlity/render/primitives.h>
#include <sh4zam/shz_sh4zam.h>

/**    0|7
 *      /\
 *     /  \
 *    /    \
 *  6/__  __\1
 *    5|  |2
 *     |  |
 *     |  |
 *     |__|
 *     4  3
 */
#define Y_OFF -6.0f

// clang-format off
const static shz_vec2_t arrow_vertices[8] = {
    {.x =  0.0f, .y = Y_OFF + -8.0f},  // 0
    {.x =  4.0f, .y = Y_OFF + -4.0f},  // 1
    {.x =  2.0f, .y = Y_OFF + -4.0f},  // 2
    {.x =  2.0f, .y = Y_OFF +  6.0f},  // 3
    {.x = -2.0f, .y = Y_OFF +  6.0f},  // 4
    {.x = -2.0f, .y = Y_OFF + -4.0f},  // 5
    {.x = -4.0f, .y = Y_OFF + -4.0f},  // 6
    {.x =  0.0f, .y = Y_OFF + -8.0f}}; // 7
// clang-format on

static inline void _render_arrow(game_player_t *player) {
  enj_color_t arrow_color = player->color;
  arrow_color.a = 0xAf;

  float offset_x = (float)((scene_t *)player->scene)->offset_x - 0.5f;
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle + 1.5708f);

  const float arrow_scale =
      1.2f + (player->shoot_power / MAX_SHOOT_POWER) * 2.5f;

  shz_vec2_t rotated_verices[8];
  for (int i = 0; i < 8; i++) {
    rotated_verices[i] = (shz_vec2_t){
        .x = arrow_scale * (arrow_vertices[i].x * barrel.cos -
                            arrow_scale * arrow_vertices[i].y * barrel.sin),
        .y = arrow_scale * (arrow_vertices[i].x * barrel.sin +
                            arrow_scale * arrow_vertices[i].y * barrel.cos),
    };
  }
  render_strip_line(
      rotated_verices, 8,
      &(shz_vec3_t){.x = offset_x + player->position.x + barrel.sin * 10.0f,
                    .y = -player->position.y + barrel.cos * 10.0f,
                    .z = 1.0f},
      1.5f, arrow_color, PVR_LIST_TR_POLY);
}

#define num_trajectory_points 1000
static inline void _render_trajectory(game_player_t *player) {
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);
  float power_scale = player->shoot_power * 10.0f;

  shz_vec2_t points[num_trajectory_points];
  float time_step = 0.5f;
  for (int i = 0; i < num_trajectory_points; i++) {
    float t = time_step * (float)i;
    points[i] = (shz_vec2_t){
        .x = player->position.x + (barrel.cos * power_scale * t),
        .y = player->position.y + (barrel.sin * power_scale * t) -
             (0.5f * 9.81f * t * t),
    };
    if (points[i].y < 0.0f) {
      return render_strip_line(
          points, i,
          &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x - 0.5f,
                        .y = 0.0f,
                        .z = 1.0f},
          1.0f,
          (enj_color_t){.r = player->color.r,
                        .g = player->color.g,
                        .b = player->color.b,
                        .a = 0x50},
          PVR_LIST_TR_POLY);
    }
  }
  render_strip_line(
      points, num_trajectory_points,
      &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x - 0.5f,
                    .y = 0.0f,
                    .z = 1.0f},
      1.0f, (enj_color_t){.raw = 0x80FFFF00}, PVR_LIST_TR_POLY);
}

static void _render_player_TR(void *data) {
  game_player_t *player = (game_player_t *)data;
  _render_arrow(player);
  _render_trajectory(player);
}

static void _render_player_OP(void *data) {
  game_player_t *player = (game_player_t *)data;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  pvr_sprite_hdr_t *hdr = (pvr_sprite_hdr_t *)pvr_dr_target(state);
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = player->color.raw;
  pvr_dr_commit(hdr);

  float offset_x = (float)((scene_t *)player->scene)->offset_x;
  enj_draw_sprite(
      (float[4][3]){
          {(player->position.x + offset_x - 4.0f) * ENJ_XSCALE,
           vid_mode->height - player->position.y - 6.0f, 1.0f},
          {(player->position.x + offset_x + 3.0f) * ENJ_XSCALE,
           vid_mode->height - player->position.y - 6.0f, 1.0f},
          {(player->position.x + offset_x + 6.0f) * ENJ_XSCALE,
           vid_mode->height - player->position.y, 1.0f},
          {(player->position.x + offset_x - 7.0f) * ENJ_XSCALE,
           vid_mode->height - player->position.y, 1.0f},
      },
      &state, NULL, NULL);
}

void render_player(game_player_t *player) {

  // calculate arrow vertices
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);

  enj_render_list_add(PVR_LIST_OP_POLY, _render_player_OP, player);
  enj_render_list_add(PVR_LIST_TR_POLY, _render_player_TR, player);
}
