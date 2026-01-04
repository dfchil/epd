#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/player.h>

const static shz_vec2_t arrow_vertices[7] = {
    {.x = -10.0f, .y = -2.0f},
    {.x = 0.0f, .y = -2.0f},
    {.x = 6.0f, .y = -6.0f},
    {.x = 8.0f, .y = -4.0f},
    {.x = 2.0f, .y = 0.0f},
    {.x = 8.0f, .y = 4.0f},
    {.x = 6.0f, .y = 6.0f},
};

static shz_vec2_t cached_vertices[7];

static void _render_player_TR(void* data) {
  game_player_t* player = (game_player_t*)data;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_TR_POLY);
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

  // calculate arrow vertices
  const shz_sincos_t barrel = shz_sincos(player->shoot_angle);



  enj_render_list_add(PVR_LIST_OP_POLY, _render_player_OP, player);
}
