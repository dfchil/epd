#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/collision.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/arrow.h>
#include <mortarlity/render/player.h>
#include <mortarlity/render/primitives.h>
#include <mortarlity/render/trajectory.h>
#include <sh4zam/shz_sh4zam.h>

static void _render_player_TR(void *data) {
  game_player_t *player = (game_player_t *)data;
  render_arrow(player);
  render_trajectory_TR(player);
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

  /**
   *   0 ______ 1
   *     \    /
   *    3 ---- 2
   */
  const float offset_x = (float)((scene_t *)player->scene)->offset_x;

  const float pmodel[4][3] = {
      {(player->position.x + offset_x - 7.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y - 1.5f, 10.0f}, // 0
      {(player->position.x + offset_x + 6.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y - 1.5f, 10.0f}, // 1
      {(player->position.x + offset_x + 3.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y + 5.0f, 10.0f}, // 2
      {(player->position.x + offset_x - 4.0f) * ENJ_XSCALE,
       vid_mode->height - player->position.y + 5.0f, 10.0f}};

  enj_draw_sprite(pmodel, &state, NULL, NULL);
}

void render_player(game_player_t *player) {
  render_arrow(player);
  // calculate arrow vertices
  enj_render_list_add(PVR_LIST_OP_POLY, _render_player_OP, player);
  enj_render_list_add(PVR_LIST_TR_POLY, _render_player_TR, player);
}
