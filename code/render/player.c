#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/collision.h>
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
    {.x =  0.0f, .y = -14.0f},  // 0
    {.x =  4.0f, .y = -10.0f},  // 1
    {.x =  2.0f, .y = -10.0f},  // 2
    {.x =  2.0f, .y =   0.0f},  // 3
    {.x = -2.0f, .y =   0.0f},  // 4
    {.x = -2.0f, .y = -10.0f},  // 5
    {.x = -4.0f, .y = -10.0f},  // 6
    {.x =  0.0f, .y = -14.0f}}; // 7
// clang-format on

static inline void _render_arrow_fill(game_player_t *player) {
  enj_color_t arrow_color = player->color.primary;
  arrow_color.a = 0x3F;

  float offset_x = (float)((scene_t *)player->scene)->offset_x - 0.5f;

  pvr_dr_state_t state;
  pvr_dr_init(&state);
  pvr_poly_cxt_t cxt;
  // mhdr_p = (pvr_mod_hdr_t *)pvr_dr_target(dr_state);
  // *mhdr_p = mhdr_p1;
  // pvr_dr_commit(mhdr_p);

  pvr_poly_cxt_col_mod(&cxt, PVR_LIST_TR_POLY);
  cxt.gen.culling = PVR_CULLING_NONE;
  pvr_poly_mod_hdr_t *hdr_m = (pvr_poly_mod_hdr_t *)pvr_dr_target(state);
  pvr_poly_mod_compile(hdr_m, &cxt);
  pvr_dr_commit(hdr_m);

  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle + 1.5708f);
  const float arrow_scale =
      1.2f + (player->shoot_power / MAX_SHOOT_POWER) * 2.5f;

  const float fillheight =
      1.0f - (player->cooldown_timer / (float)SHOT_COOLDOWN_FRAMES);

  shz_vec2_t box_vertices[4] = {
      {.x = -4.0f, .y = Y_OFF + 6.0f},
      {.x = -4.0f, .y = (Y_OFF - 8.0f) * fillheight},
      {.x = 4.0f, .y = Y_OFF + 6.0f},
      {.x = 4.0f, .y = (Y_OFF - 8.0f) * fillheight},
  };

  for (int i = 0; i < 4; i++) {
    shz_vec2_t rotated = {
        .x = barrel.sin * 10.0f +
             (arrow_scale * (box_vertices[i].x * barrel.cos -
                             arrow_scale * box_vertices[i].y * barrel.sin)),
        .y = -barrel.cos * 10.0f +
             (arrow_scale * (box_vertices[i].x * barrel.sin +
                             arrow_scale * box_vertices[i].y * barrel.cos))};
    box_vertices[i] = (shz_vec2_t){
        .x = rotated.x + player->position.x + offset_x,
        .y = rotated.y + player->position.y,
    };
    pvr_vertex_pcm_t *vert = (pvr_vertex_pcm_t *)pvr_dr_target(state);
    vert->flags = i == 3 ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
    vert->x = box_vertices[i].x * ENJ_XSCALE;
    vert->y = vid_mode->height - box_vertices[i].y;
    vert->z = 1.0f;
    vert->argb0 = 0;
    vert->argb1 = arrow_color.raw;
    pvr_dr_commit(vert);
  }
  pvr_dr_finish();
}

static inline void _render_arrow(game_player_t *player) {

#define ARROW_LINE_THICKNESS 2.0f
  float offset_x = (float)((scene_t *)player->scene)->offset_x - 0.5f;

  const float fillheight =
      1.0f - (player->cooldown_timer / (float)SHOT_COOLDOWN_FRAMES);

  if (player->cooldown_timer == 0 ||
      player->cooldown_timer >= SHOT_COOLDOWN_FRAMES) {
    enj_color_t arrow_color = player->cooldown_timer == 0
                                  ? player->color.primary
                                  : player->color.contrast;
    arrow_color.a = 0xAf;
    render_strip_line(player->arrow_vertices, 8,
                      &(shz_vec3_t){.x = offset_x + player->position.x,
                                    .y = -player->position.y,
                                    .z = 1.0f},
                      ARROW_LINE_THICKNESS, arrow_color, PVR_LIST_TR_POLY);
  } else {
    enj_color_t prim_color = player->color.primary;
    prim_color.a = 0xAf;
    enj_color_t contrast_color = player->color.contrast;
    contrast_color.a = 0xAf;

    const float fillheight =
        1.0f - (player->cooldown_timer / (float)SHOT_COOLDOWN_FRAMES);
    const float arrow_scale =
        1.2f + (player->shoot_power / MAX_SHOOT_POWER) * 2.5f;

    shz_vec2_t cut_from = {.x = -5.0f, .y = arrow_vertices[0].y * fillheight};
    shz_vec2_t cut_to = {.x = 5.0f, .y = arrow_vertices[0].y * fillheight};
    if (fillheight * arrow_vertices[0].y <= arrow_vertices[1].y) {
      // calculate intersection interpolation
      float cut_intr = collision_line_line(
          &arrow_vertices[0],
          &(shz_vec2_t){.x = arrow_vertices[1].x - arrow_vertices[0].x,
                        .y = arrow_vertices[1].y - arrow_vertices[0].y},
          &cut_from,
          &(shz_vec2_t){.x = cut_to.x - cut_from.x,
                        .y = cut_to.y - cut_from.y});

      // cut off before point 1
      shz_vec2_t top_vertices[3];
      top_vertices[0] = shz_vec2_lerp(player->arrow_vertices[7],
                                      player->arrow_vertices[6], cut_intr);
      top_vertices[1] = player->arrow_vertices[7];
      top_vertices[2] = shz_vec2_lerp(player->arrow_vertices[0],
                                      player->arrow_vertices[1], cut_intr);

      render_strip_line(top_vertices, 3,
                        &(shz_vec3_t){.x = offset_x + player->position.x,
                                      .y = -player->position.y,
                                      .z = 1.0f},
                        ARROW_LINE_THICKNESS, contrast_color, PVR_LIST_TR_POLY);
      shz_vec2_t bottom_vertices[8];
      bottom_vertices[0] = top_vertices[2];
      for (int i = 1; i < 7; i++) {
        bottom_vertices[i] = player->arrow_vertices[i];
      }
      bottom_vertices[7] = top_vertices[0];
      render_strip_line(bottom_vertices, 8,
                        &(shz_vec3_t){.x = offset_x + player->position.x,
                                      .y = -player->position.y,
                                      .z = 1.0f},
                        ARROW_LINE_THICKNESS, prim_color, PVR_LIST_TR_POLY);

    } else {
      float cut_intr = collision_line_line(
          &arrow_vertices[2],
          &(shz_vec2_t){.x = arrow_vertices[3].x - arrow_vertices[2].x,
                        .y = arrow_vertices[3].y - arrow_vertices[2].y},
          &cut_from,
          &(shz_vec2_t){.x = cut_to.x - cut_from.x,
                        .y = cut_to.y - cut_from.y});
      shz_vec2_t top_vertices[7];
      top_vertices[0] = shz_vec2_lerp(
          player->arrow_vertices[4], player->arrow_vertices[5], 1.0 - cut_intr);
      top_vertices[1] = player->arrow_vertices[5];
      top_vertices[2] = player->arrow_vertices[6];
      top_vertices[3] = player->arrow_vertices[0];
      top_vertices[4] = player->arrow_vertices[1];
      top_vertices[5] = player->arrow_vertices[2];
      top_vertices[6] = shz_vec2_lerp(player->arrow_vertices[2],
                                      player->arrow_vertices[3], cut_intr);
      render_strip_line(top_vertices, 7,
                        &(shz_vec3_t){.x = offset_x + player->position.x,
                                      .y = -player->position.y,
                                      .z = 1.0f},
                        ARROW_LINE_THICKNESS, contrast_color, PVR_LIST_TR_POLY);
      shz_vec2_t bottom_vertices[4];
      bottom_vertices[0] = top_vertices[6];
      bottom_vertices[1] = player->arrow_vertices[3];
      bottom_vertices[2] = player->arrow_vertices[4];
      bottom_vertices[3] = top_vertices[0];
      render_strip_line(bottom_vertices, 4,
                        &(shz_vec3_t){.x = offset_x + player->position.x,
                                      .y = -player->position.y,
                                      .z = 1.0f},
                        ARROW_LINE_THICKNESS, prim_color, PVR_LIST_TR_POLY);
    }
  }
}

#define num_trajectory_points 1000
static inline void _render_trajectory(game_player_t *player) {
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);
  float power_scale = player->shoot_power * 10.0f;

  shz_vec2_t points[num_trajectory_points];
  float time_step = 0.5f;

  shz_vec2_t fire_start = {
      .x = player->position.x + (barrel.cos * 10.0f),
      .y = player->position.y + (barrel.sin * 10.0f),
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
          &(shz_vec3_t){.x = (float)((scene_t *)player->scene)->offset_x - 0.5f,
                        .y = 0.0f,
                        .z = 1.0f},
          1.0f,
          (enj_color_t){.r = player->color.primary.r,
                        .g = player->color.primary.g,
                        .b = player->color.primary.b,
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

static void _render_player_TEST(void *data);
static void _render_player_TR(void *data) {
  game_player_t *player = (game_player_t *)data;
  _render_arrow(player);
  // _render_trajectory(player);
  // _render_arrow_fill(player);
  // _render_player_TEST(data);
}

static void _render_player_OP(void *data) {
  game_player_t *player = (game_player_t *)data;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  pvr_sprite_hdr_t *hdr = (pvr_sprite_hdr_t *)pvr_dr_target(state);
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = player->color.primary.raw;
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

static void _render_player_MOD_TR(void *data) {
  pvr_mod_hdr_t mhdr_p0;
  pvr_mod_compile(&mhdr_p0, PVR_LIST_TR_MOD, PVR_MODIFIER_OTHER_POLY,
                  PVR_CULLING_CW);

  pvr_mod_hdr_t mhdr_p1;
  pvr_mod_compile(&mhdr_p1, PVR_LIST_TR_MOD, PVR_MODIFIER_INCLUDE_LAST_POLY,
                  PVR_CULLING_CW);

  game_player_t *player = (game_player_t *)data;

  pvr_dr_state_t dr_state;
  pvr_dr_init(&dr_state);
  pvr_mod_hdr_t *mhdr_p = (pvr_mod_hdr_t *)pvr_dr_target(dr_state);
  *mhdr_p = mhdr_p1;
  pvr_dr_commit(mhdr_p);

  float offset_x =
      (float)((scene_t *)player->scene)->offset_x - 0.5f + player->position.x;
  float offset_y = (float)player->position.y;

  pvr_modifier_vol_t *modh_p = (pvr_modifier_vol_t *)pvr_dr_target(dr_state);
  modh_p->flags = PVR_CMD_VERTEX_EOL;
  modh_p->ax = (player->arrow_vertices[0].x + offset_x) * ENJ_XSCALE;
  modh_p->ay = vid_mode->height - (player->arrow_vertices[0].y + offset_y);
  modh_p->az = 22.11f;
  modh_p->bx = (player->arrow_vertices[1].x + offset_x) * ENJ_XSCALE;
  modh_p->by = vid_mode->height - (player->arrow_vertices[1].y + offset_y);
  modh_p->bz = 22.11f;
  modh_p->cx = (player->arrow_vertices[6].x + offset_x) * ENJ_XSCALE;
  pvr_dr_commit(modh_p);
  modh_p = (pvr_modifier_vol_t *)pvr_dr_target(dr_state);
  pvr_modifier_vol_t *modh_p2 = (pvr_modifier_vol_t *)((int)modh_p - 32);
  modh_p2->cy = vid_mode->height - (player->arrow_vertices[6].y + offset_y);
  modh_p2->cz = 22.11f;
  pvr_dr_commit(modh_p);

  // mhdr_p = (pvr_mod_hdr_t *)pvr_dr_target(dr_state);
  // *mhdr_p = mhdr_p0;
  // pvr_dr_commit(mhdr_p);

  modh_p = (pvr_modifier_vol_t *)pvr_dr_target(dr_state);
  modh_p->flags = PVR_CMD_VERTEX_EOL;
  modh_p->ax = (player->arrow_vertices[4].x + offset_x) * ENJ_XSCALE;
  modh_p->ay = vid_mode->height - (player->arrow_vertices[4].y + offset_y);
  modh_p->az = 22.11f;
  modh_p->bx = (player->arrow_vertices[5].x + offset_x) * ENJ_XSCALE;
  modh_p->by = vid_mode->height - (player->arrow_vertices[5].y + offset_y);
  modh_p->bz = 22.11f;
  modh_p->cx = (player->arrow_vertices[3].x + offset_x) * ENJ_XSCALE;
  pvr_dr_commit(modh_p);
  modh_p = (pvr_modifier_vol_t *)pvr_dr_target(dr_state);
  modh_p2 = (pvr_modifier_vol_t *)((int)modh_p - 32);
  modh_p2->cy = vid_mode->height - (player->arrow_vertices[3].y + offset_y);
  modh_p2->cz = 22.11f;
  pvr_dr_commit(modh_p);

  // mhdr_p = (pvr_mod_hdr_t *)pvr_dr_target(dr_state);
  // *mhdr_p = mhdr_p1;
  // pvr_dr_commit(mhdr_p);

  modh_p = (pvr_modifier_vol_t *)pvr_dr_target(dr_state);
  modh_p->flags = PVR_CMD_VERTEX_EOL;
  modh_p->ax = (player->arrow_vertices[2].x + offset_x) * ENJ_XSCALE;
  modh_p->ay = vid_mode->height - (player->arrow_vertices[2].y + offset_y);
  modh_p->az = 22.11f;
  modh_p->bx = (player->arrow_vertices[3].x + offset_x) * ENJ_XSCALE;
  modh_p->by = vid_mode->height - (player->arrow_vertices[3].y + offset_y);
  modh_p->bz = 22.11f;
  modh_p->cx = (player->arrow_vertices[5].x + offset_x) * ENJ_XSCALE;
  pvr_dr_commit(modh_p);
  modh_p = (pvr_modifier_vol_t *)pvr_dr_target(dr_state);
  modh_p2 = (pvr_modifier_vol_t *)((int)modh_p - 32);
  modh_p2->cy = vid_mode->height - (player->arrow_vertices[5].y + offset_y);
  modh_p2->cz = 22.11f;
  pvr_dr_commit(modh_p);

  pvr_dr_finish();
}

void render_player(game_player_t *player) {
  // calculate arrow vertices
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle + 1.5708f);
  const float arrow_scale =
      1.2f + (player->shoot_power / MAX_SHOOT_POWER) * 2.5f;

  for (int i = 0; i < 7; i++) {
    player->arrow_vertices[i] = (shz_vec2_t){
        .x = barrel.sin * 10.0f +
             (arrow_scale * (arrow_vertices[i].x * barrel.cos -
                             arrow_scale * arrow_vertices[i].y * barrel.sin)),
        .y = -barrel.cos * 10.0f +
             (arrow_scale * (arrow_vertices[i].x * barrel.sin +
                             arrow_scale * arrow_vertices[i].y * barrel.cos)),
    };
  }
  player->arrow_vertices[7] = player->arrow_vertices[0];

  enj_render_list_add(PVR_LIST_OP_POLY, _render_player_OP, player);
  enj_render_list_add(PVR_LIST_TR_POLY, _render_player_TR, player);
  // enj_render_list_add(PVR_LIST_TR_MOD, _render_player_MOD_TR, player);
}
