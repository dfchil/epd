
#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/collision.h>
#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/arrow.h>
#include <mortarlity/render/primitives.h>
#include <sh4zam/shz_sh4zam.h>


#define ARROW_LINE_THICKNESS 2.0f

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


void _render_arrow_TR(void *data) {
  game_player_t *player = (game_player_t *)data;

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
      // cut off before point 1

      // calculate intersection interpolation
      float cut_intr = collision_line_line(
          &arrow_vertices[0],
          &(shz_vec2_t){.x = arrow_vertices[1].x - arrow_vertices[0].x,
                        .y = arrow_vertices[1].y - arrow_vertices[0].y},
          &cut_from,
          &(shz_vec2_t){.x = cut_to.x - cut_from.x,
                        .y = cut_to.y - cut_from.y});

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


void render_arrow(game_player_t *player) {
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
  enj_render_list_add(
      PVR_LIST_TR_POLY, _render_arrow_TR, (void *)player);
}
