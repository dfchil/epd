#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/player.h>
#include <mortarlity/render/primitives.h>
#include <sh4zam/shz_sh4zam.h>

/**
 *   /\
 *  /  \
 * /    \
 * --  --
 *  |  |
 *  |  |
 *  |  |
 *  |  |
 *  ----
 *
 */

#define Y_OFF -10.0f

const static shz_vec2_t arrow_vertices[8] = {
    {.x = 0.0f, .y = Y_OFF + -8.0f},  {.x = 4.0f, .y = Y_OFF + -4.0f},
    {.x = 2.0f, .y = Y_OFF + -4.0f},  {.x = 2.0f, .y = Y_OFF + 6.0f},
    {.x = -2.0f, .y = Y_OFF + 6.0f},  {.x = -2.0f, .y = Y_OFF + -4.0f},
    {.x = -4.0f, .y = Y_OFF + -4.0f}, {.x = 0.0f, .y = Y_OFF + -8.0f}};

static void _render_player_TR(void *data) {
  game_player_t *player = (game_player_t *)data;

  enj_color_t arrow_color = player->color;
  arrow_color.a = 0xAf;

  float offset_x = (float)((scene_t *)player->scene)->offset_x;
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle + 1.5708f);

  const float arrow_scale = 1.5f;

  shz_vec2_t rotated_verices[8];
  for (int i = 0; i < 8; i++) {
    rotated_verices[i] = (shz_vec2_t){
        .x = offset_x + player->position.x +
             arrow_scale * (arrow_vertices[i].x * barrel.cos -
                            arrow_scale * arrow_vertices[i].y * barrel.sin),
        .y = (player->position.y +
              arrow_scale * (arrow_vertices[i].x * barrel.sin +
                             arrow_scale * arrow_vertices[i].y * barrel.cos)),
    };
  }
  render_strip_line(rotated_verices, 8, 1.5f, 1.5f, arrow_color,
                    PVR_LIST_TR_POLY);
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
