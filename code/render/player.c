#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/player.h>

static void _render_player_OP(void* data) {
  game_player_t* player = (game_player_t*)data;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(state);
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = player->color.raw;
  pvr_dr_commit(hdr);

  float offset_x = (float)((scene_t*)player->scene)->offset_x * ENJ_XSCALE;
  
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
