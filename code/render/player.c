#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/player.h>
#include <gorgol8/render/player.h>

static void _render_player_OP(void* data) {
  // scene_t* a_scene = scene_get_active();

  game_player_t* player = (game_player_t*)data;
  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(state);
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = player->color.raw;
  pvr_dr_commit(hdr);

  // float offset_x = (float)a_scene->offset_x * ENJ_XSCALE;
  float offset_x = 0.0f;

  enj_draw_sprite(
      (float[4][3]){
          {player->position.x + offset_x - 4.0f * ENJ_XSCALE,
           vid_mode->height - player->position.y - 6.0f, 1.0f},
          {player->position.x + offset_x + 3.0f * ENJ_XSCALE,
           vid_mode->height - player->position.y - 6.0f, 1.0f},
          {player->position.x + offset_x + 6.0f * ENJ_XSCALE,
           vid_mode->height - player->position.y, 1.0f},
          {player->position.x + offset_x - 7.0f * ENJ_XSCALE,
           vid_mode->height - player->position.y, 1.0f},
      },
      &state, NULL, NULL);
}

void render_player(game_player_t* player) {
  enj_render_list_add(PVR_LIST_OP_POLY, _render_player_OP, player);
}
