
#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/collision.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/arrow.h>
#include <mortarlity/render/primitives.h>
#include <sh4zam/shz_sh4zam.h>

#define ARROW_LINE_THICKNESS 1.f

/**     5
 *      /\
 *     /  \
 *    /    \
 *  6/__  __\4
 *    7|  |3
 *     |  |
 *  8|0|  |
 *     |__|
 *     1  2
 */

// clang-format off
alignas(32) const static shz_vec2_t arrow_vertices[9] = {
    {.x =  2.0f, .y = - 5.0f},  // 0
    {.x =  2.0f, .y =   0.0f},  // 1
    {.x = -2.0f, .y =   0.0f},  // 2
    {.x = -2.0f, .y = -10.0f},  // 3
    {.x = -4.0f, .y = -9.85f},  // 4
    {.x =  0.0f, .y = -14.0f},  // 5
    {.x =  4.0f, .y = -9.85f},  // 6
    {.x =  2.0f, .y = -10.0f},  // 7
    {.x =  2.0f, .y = - 5.0f},  // 8
  };
// clang-format on

    void _render_arrow_OP(void *data) {
  game_player_t *player = (game_player_t *)data;

  float offset_x = (float)((scene_t *)player->scene)->offset_x - 0.5f;

  const float fillheight =
      1.0f - (player->cooldown_timer / (float)SHOT_COOLDOWN_FRAMES);
  enj_color_t prim_color = player->color.primary;
  enj_color_t contrast_color = player->color.contrast;
  contrast_color.a = 0xAf;
  render_strip_line(player->arrow_vertices, 9,
                    &(shz_vec3_t){.x = offset_x + player->position.x,
                                  .y = -player->position.y,
                                  .z = 1.0f},
                    ARROW_LINE_THICKNESS, prim_color, PVR_LIST_OP_POLY, NULL);

  if (player->cooldown_timer != 0 &&
      player->cooldown_timer <= SHOT_COOLDOWN_FRAMES) {
    const float fillheight =
        1.0f - (player->cooldown_timer / (float)SHOT_COOLDOWN_FRAMES);
    alignas(32) float zvalues[9];
    for (int i = 0; i < 9; i++) {
      zvalues[i] = 100.0f * arrow_vertices[i].y / arrow_vertices[5].y;
    }
    render_strip_line(player->arrow_vertices, 9,
                      &(shz_vec3_t){.x = offset_x + player->position.x,
                                    .y = -player->position.y,
                                    .z = -100.0f * fillheight},
                      ARROW_LINE_THICKNESS, contrast_color, PVR_LIST_TR_POLY,
                      zvalues);
  }
}

void render_arrow(game_player_t *player) {
  const shz_sincos_t barrel = shz_sincosf(player->shoot_angle + 1.5708f);
  const float arrow_scale =
      1.2f + (player->shoot_power / MAX_SHOOT_POWER) * 2.5f;

  for (int i = 0; i < 8; i++) {
    player->arrow_vertices[i] = (shz_vec2_t){
        .x = barrel.sin * BARREL_OFFSET +
             (arrow_scale * (arrow_vertices[i].x * barrel.cos -
                             arrow_scale * arrow_vertices[i].y * barrel.sin)),
        .y = -barrel.cos * BARREL_OFFSET +
             (arrow_scale * (arrow_vertices[i].x * barrel.sin +
                             arrow_scale * arrow_vertices[i].y * barrel.cos)),
    };
  }
  player->arrow_vertices[8] = player->arrow_vertices[0];
  enj_render_list_add(PVR_LIST_OP_POLY, _render_arrow_OP, (void *)player);
}
